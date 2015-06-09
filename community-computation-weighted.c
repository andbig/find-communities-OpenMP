#include "community-computation-weighted.h"
#include "temporary-community-edge.h"

#include "community-development.h"
#include "dynamic-weighted-graph.h"
#include "community-exchange.h"
#include "silent-switch.h"
#include "sorted-linked-list.h"

#include <omp.h>

#ifdef SILENT_SWITCH_ON
#define printf(...)
#endif

#define NODE_ITERATION_CHUNK_SIZE 10
#define EXCHANGE_CHUNK_SIZE 10

// Designed to be called at the start of each phase
int community_developer_init_weighted(community_developer *cd, dynamic_weighted_graph *dwg) {
	int i;
	int accumulated_edge_number;
	int node_degree;

	cd->n = dwg->size;
	cd->double_m = 0;

	// Number of vertex neighbor communities doesn't need to be initialized, since it is overwritten when a node is considered

	if(!(cd->vertex_community = (int *) malloc(cd->n * sizeof(int))) ||
			!(cd->internal_weight_community = (int *) malloc(cd->n * sizeof(int))) ||
			!(cd->incoming_weight_community = (int *) malloc(cd->n * sizeof(int))) ||
			!(cd->incoming_weight_node = (int *) malloc(cd->n * sizeof(int))) ||
			!(cd->cumulative_edge_number = (int *) malloc(cd->n * sizeof(int))) ||
			!(cd->vertex_neighbor_communities = (int *) malloc(cd->n * sizeof(int)))) {

		printf("Out of memory!\n");
		community_developer_free(cd);

		return 0;
	}

	accumulated_edge_number = 0;

	for(i = 0; i < cd->n; i++) {
		// Put each node in an individual community
		*(cd->vertex_community+i) = i;
		*(cd->internal_weight_community+i) = (dwg->edges+i)->self_loop;
		cd->double_m += (dwg->edges+i)->self_loop;

		node_degree = dynamic_weighted_graph_node_degree(dwg, i);
		*(cd->incoming_weight_community+i) = *(cd->incoming_weight_node+i) = node_degree;
		cd->double_m += node_degree;

		accumulated_edge_number += (dwg->edges+i)->count;
		*(cd->cumulative_edge_number+i) = accumulated_edge_number;
	}

	if(!(cd->exchange_ranking = (community_exchange *) malloc(accumulated_edge_number * sizeof(community_exchange)))){
		printf("Out of memory!\n");
		community_developer_free(cd);

		return 0;
	}


	return 1;
}

double removal_modularity_loss_weighted(dynamic_weighted_graph *dwg, community_developer *cd, int node_index, int k_i_in) {
	// I assume the removal loss is equal to the gain that we would get by
	// putting the node back in its former community.

	modularity_computing_package mcp;

	// TODO
	get_modularity_computing_package(&mcp, cd, node_index, *(cd->vertex_community+node_index), k_i_in);

	mcp.sum_tot -= k_i_in;
	mcp.sum_in = mcp.sum_in - 2 * k_i_in - (dwg->edges + node_index)->self_loop;

	return modularity_delta(&mcp);
}

// Note: Current community not included in sll. Internal links are considered and their sum is put into current_community_k_i_in. Self loops are ignored
int get_neighbor_communities_list_weighted(sorted_linked_list *sll, dynamic_weighted_graph *dwg, community_developer *cd, int node_index, int *current_community_k_i_in) {
	dynamic_weighted_edge_array *neighbor_nodes;
	int i;

	int current_community;
	*current_community_k_i_in = 0;

	if(!dwg || !sll) {
		printf("Invalid input in computation of neighbor communities!");
		return 0;
	}

	if(node_index < 0 || node_index >= dwg->size) {
		printf("Invalid input in computation of neighbor communities: node out of range!");
		return 0;
	}

	current_community = *(cd->vertex_community + node_index);

	sorted_linked_list_free(sll);

	neighbor_nodes = dwg->edges + node_index;

//	Self loop not included in k_i_in of current community

//	if(neighbor_nodes->self_loop)
//		if(!(sorted_linked_list_insert(sll, *(cd->vertex_community+node_index), neighbor_nodes->self_loop))) {
//			printf("Cannot insert node in sorted linked list!");
//
//			sorted_linked_list_free(sll);
//			free(sll);
//
//			return NULL;
//		}

	for(i = 0; i < neighbor_nodes->count; i++) {
		if(*(cd->vertex_community + (neighbor_nodes->addr + i)->dest) == current_community)
			// Link internal to current node community
			*current_community_k_i_in += (neighbor_nodes->addr + i)->weight;
		else if(!(sorted_linked_list_insert(sll, *(cd->vertex_community + (neighbor_nodes->addr + i)->dest), (neighbor_nodes->addr + i)->weight))) {
			printf("Cannot insert node in sorted linked list!");

			sorted_linked_list_free(sll);
			free(sll);

			return 0;
		}
	}

	return 1;
}

inline double compute_modularity_edge_weighted(dynamic_weighted_graph *dwg, int edge_weight, int src_incoming_weight, int dest_incoming_weight, int double_m, int src_community, int dest_community) {
	return ((double) edge_weight - (double) src_incoming_weight * dest_incoming_weight / double_m) * same(src_community,dest_community);
}

double compute_modularity_community_vector_weighted(dynamic_weighted_graph *dwg, int *community_vector) {
	// TODO parallelize

	int *incoming_weight;
	int i,j;
	int src, dest, src_incoming_weight, dest_incoming_weight, src_community, dest_community, src_self_loop, edge_weight;
	int double_m;
	double result;

	if(!dwg || !community_vector) {
		printf("Invalid input for modularity computation!\n");

		return ILLEGAL_MODULARITY_VALUE;
	}

	if(!(incoming_weight = (int*) malloc(dwg->size * sizeof(int)))) {
		printf("Couldn't allocate memory needed for modularity computation!\n");

		return ILLEGAL_MODULARITY_VALUE;
	}

	for(i = 0; i < dwg->size; i++)
		*(incoming_weight + i) = dynamic_weighted_graph_node_degree(dwg, i);

	double_m = dynamic_weighted_graph_double_m(dwg);

	result = 0;

	for(src=0;src<dwg->size;src++) {
		src_incoming_weight = *(incoming_weight + src);
		src_community = *(community_vector + src);
		src_self_loop = (dwg->edges + src)->self_loop;

		for(j=0;j<(dwg->edges+src)->count;j++) {
			dest = ((dwg->edges+src)->addr+j)->dest;
			edge_weight = ((dwg->edges+src)->addr+j)->weight;
			dest_incoming_weight = *(incoming_weight + dest);
			dest_community = *(community_vector + dest);

//			result += ((double) edge_weight - (double) src_incoming_weight * dest_incoming_weight / cd->double_m) * same(src_community,dest_community);
			result += compute_modularity_edge_weighted(dwg, edge_weight, src_incoming_weight, dest_incoming_weight, double_m, src_community, dest_community);
		}

//		result += ((double) src_self_loop - (double) src_incoming_weight * src_incoming_weight / cd->double_m);
		result += compute_modularity_edge_weighted(dwg, src_self_loop, src_incoming_weight, src_incoming_weight, double_m, src_community, src_community);
	}

	result /= double_m;

	free(incoming_weight);

	return result;
}

double compute_modularity_weighted(dynamic_weighted_graph *dwg, community_developer *cd) {
	double result;
	int src,j;
	int dest, edge_weight;

	int src_incoming_weight;
	int dest_incoming_weight;

	int src_community, dest_community;
	int src_self_loop;

	if(!dwg || !cd){
		printf("Invalid input: cannot compute modularity!");
		return ILLEGAL_MODULARITY_VALUE;
	}

	result = 0;

	for(src=0;src<dwg->size;src++) {
		src_incoming_weight = *(cd->incoming_weight_node+src);
		src_community = *(cd->vertex_community+src);
		src_self_loop = (dwg->edges + src)->self_loop;

		for(j=0;j<(dwg->edges+src)->count;j++) {
			dest = ((dwg->edges+src)->addr+j)->dest;
			edge_weight = ((dwg->edges+src)->addr+j)->weight;
			dest_incoming_weight = *(cd->incoming_weight_node+dest);
			dest_community = *(cd->vertex_community+dest);

			result += compute_modularity_edge_weighted(dwg, edge_weight, src_incoming_weight, dest_incoming_weight, cd->double_m, src_community, dest_community);
		}

		result += compute_modularity_edge_weighted(dwg, src_self_loop, src_incoming_weight, src_incoming_weight, cd->double_m, src_community, src_community);
	}

	result /= cd->double_m;

	return result;
}

inline void apply_transfer_weighted(dynamic_weighted_graph *dwg, community_developer *cd, community_exchange *exchange) {
	const int src =  *(cd->vertex_community + exchange->node);
	const int self_loop =  (dwg->edges + exchange->node)->self_loop;

	// Update source community sum_tot (incoming_weight_community)
	*(cd->incoming_weight_community + src) -= *(cd->incoming_weight_node + exchange->node);
	// Update source community sum_int (internal_weight_community)
	*(cd->internal_weight_community + src) -= (2 * exchange->k_i_in_src + self_loop);
	// Update destination community sum_tot (incoming_weight_community)
	*(cd->incoming_weight_community + exchange->dest) += *(cd->incoming_weight_node + exchange->node);
	// Update destination community sum_int (internal_weight_community)
	*(cd->internal_weight_community + exchange->dest) += (2 * exchange->k_i_in_dest + self_loop);

	*(cd->vertex_community + exchange->node) = exchange->dest;

	printf("Moving vertex %d from community %d to community %d.\n", exchange->node, src, exchange->dest);
}

int output_translator_weighted(dynamic_weighted_graph *dwg, community_developer *cd, dynamic_weighted_graph **community_graph, int **community_vector) {

	// TODO free output, just in case

	int *renumber_community;
	int i,j;

	int current_node;

	int community_of_current_node;

	int total_communities = 0;
	int community;

	temporary_community_edge *tce;

	dynamic_weighted_edge_array neighbors;

	weighted_edge neighbor;

	sorted_linked_list_elem *neighbor_community;

	printf("\n\n------------- OUTPUT TRANSLATION ------------\n\n");

	printf("Input graph:\n\n");

	dynamic_weighted_graph_print(*dwg);

	printf("\n\nInput CD:\n\n");

	community_developer_print(cd,0);

	if(!(*community_graph = (dynamic_weighted_graph *) malloc (sizeof(dynamic_weighted_graph)))) {
		printf("Cannot allocate output community graph!");

		return 0;
	}

	if(!(*community_vector = (int *) malloc (cd->n * sizeof(int)))) {
		printf("Cannot allocate non_empty_community community graph!");

		free(*community_graph);
		return 0;
	}

	if(!(renumber_community = (int *) malloc (cd->n * sizeof(int)))) {
		printf("Cannot allocate renumber_community community graph!");

		free(*community_graph);
		free(*community_vector);
		return 0;
	}

//	#pragma omp parallel for scheduling(dynamic,10) shared(non_empty_community) private(i)
	for(i = 0; i < cd->n ; i++)
		*(renumber_community+i) = -1;

//	#pragma omp parallel for scheduling(dynamic,10) shared(non_empty_community) private(i, community)
	for(i = 0; i < cd->n ; i++) {
		community = *(cd->vertex_community + i);

		if(*(renumber_community+community) != -1)
			// Community is already re-numbered
			*(*community_vector+i) = *(renumber_community+community);
		else {
			// Community isn't re-numbered yet
			*(renumber_community+community) = total_communities;
			*(*community_vector+i) = total_communities;

			total_communities++;
		}
	}

	free(renumber_community);

	dynamic_weighted_graph_init(*community_graph, total_communities);

	if(!(tce = (temporary_community_edge*) malloc(total_communities * sizeof(temporary_community_edge)))) {
		printf("Cannot allocate temporary_community_edge community graph!");

		free(*community_graph);
		free(*community_vector);
		return 0;
	}

//	#pragma omp parallel for schedule(dynamic,10) private(i)
	for(i = 0; i < total_communities; i++)
		temporary_community_edge_init(tce+i);

	// -------------------------------- SEQUENTIAL SECTION

	for(current_node = 0; current_node < cd->n ; current_node++) {
		neighbors = *(dwg->edges + current_node);
		community_of_current_node = *(*community_vector+current_node);

		for(j = 0; j < neighbors.count; j++){
			neighbor = *(neighbors.addr+j);

			if(*(*community_vector+neighbor.dest) == community_of_current_node)
				// Community selfloop
				(tce+community_of_current_node)->self_loop += neighbor.weight;
			else
				sorted_linked_list_insert(&((tce+community_of_current_node)->sll), *(*community_vector+neighbor.dest), neighbor.weight);
		}
	}

//	#pragma omp parallel for schedule(dynamic,5) private(i)
	for(i = 0; i < total_communities; i++) {
		((*community_graph)->edges + i)->self_loop = (tce+i)->self_loop;

		neighbor_community = ((tce+i)->sll).head;
		while(neighbor_community) {
			dynamic_weighted_graph_insert(*community_graph, i, neighbor_community->community, neighbor_community->k_i_in);

			neighbor_community = neighbor_community->next;
		}
	}

	temporary_community_edge_free(tce);
	free(tce);

	return 1;
}

// Executes a phase of the algorithm, returns modularity gain
double phase_weighted(dynamic_weighted_graph *dwg, double minimum_improvement, dynamic_weighted_graph **community_graph, int **community_vector) {
	community_developer cd;

	double initial_phase_modularity, final_phase_modularity;
	double initial_iteration_modularity, final_iteration_modularity;

	sorted_linked_list neighbors;
	sorted_linked_list_elem *neighbor;
	modularity_computing_package mcp;
	double to_neighbor_modularity_delta;
	int current_community_k_i_in;
	double removal_loss;
	double gain;
	// Number of neighbor communities of a node worth considering for potential node transfer
	int number_of_neighbor_communities;

	community_exchange *node_exchanges_base_pointer;

	int base,total_exchanges;

	community_exchange *sorted_output_multi_thread;
	// True if it is actually used (multi-threading)
	int sorted_output_multi_thread_needs_free;

	short *selected;
	int stop_scanning_position;

	int i,j;

	int phase_iteration_counter = 0;

	int neighbor_communities_bad_computation;

	if(!dwg || minimum_improvement < MINIMUM_LEGAL_IMPROVEMENT || minimum_improvement > MAXIMUM_LEGAL_IMPROVEMENT) {
		printf("Invalid phase parameters!");

		return ILLEGAL_MODULARITY_VALUE;
	}

	community_developer_init_weighted(&cd, dwg);

	community_developer_print(&cd,0);

	initial_phase_modularity = compute_modularity_weighted(dwg, &cd);
	final_iteration_modularity = initial_phase_modularity;

	printf("\n\n---------------- Phase start -----------------\n\n");
	printf("Initial phase modularity: %f\n", initial_phase_modularity);

	do {
		printf("\n\n---------------- Iteration #%d -----------------\n\n", phase_iteration_counter);

		sorted_output_multi_thread_needs_free = 0;

		initial_iteration_modularity = final_iteration_modularity;

		neighbor_communities_bad_computation = 0;

		printf("Initial iteration modularity: %f\n\n", initial_iteration_modularity);

		// Do the parallel iterations over all nodes
		// TODO put Parallel for - Everything else is good to go
//#pragma omp parallel for default(shared) schedule(dynamic,NODE_ITERATION_CHUNK_SIZE) private(i, node_exchanges_base_pointer, number_of_neighbor_communities,neighbors, current_community_k_i_in, removal_loss, neighbor, mcp, to_neighbor_modularity_delta, gain) reduction(||:neighbor_communities_bad_computation)
		for(i = 0; i < dwg->size; i++) {

			printf("NODE ITERATION - Thread #%d on node %d\n", omp_get_thread_num(), i);

			if(i > 0)
				node_exchanges_base_pointer = cd.exchange_ranking + *(cd.cumulative_edge_number + i - 1);
			else
				node_exchanges_base_pointer = cd.exchange_ranking;

			// Node i, current edges: dwg->edges + i

			number_of_neighbor_communities = 0;
			sorted_linked_list_init(&neighbors);

			// Compute neighbor communities and k_i_in
			if(get_neighbor_communities_list_weighted(&neighbors,dwg,&cd,i,&current_community_k_i_in)) {


				removal_loss = removal_modularity_loss_weighted(dwg, &cd, i, current_community_k_i_in);

				neighbor = neighbors.head;
				while(neighbor){
					// Note that by the definition of get_neighbor_communities_list_weighted current community isn't in the list

					// Compute modularity variation that would be achieved my moving node i from its current community to the neighbor community considered (neighbor->community)
					get_modularity_computing_package(&mcp,&cd,i,neighbor->community,neighbor->k_i_in);
					to_neighbor_modularity_delta = modularity_delta(&mcp);

					// Check if the gain is worth to be considered
					gain = to_neighbor_modularity_delta - removal_loss;
					if(gain > MINIMUM_TRANSFER_GAIN) {
						// Put computed potential transfer in its correct position for later community pairing selection

						// TODO CRITICAL Shouldn't it be cd.cumulative_edge_number + i - 1 ?
						set_exchange_ranking(node_exchanges_base_pointer + number_of_neighbor_communities,i,neighbor->community,current_community_k_i_in, neighbor->k_i_in,gain);

						number_of_neighbor_communities += 1;
					}

					neighbor = neighbor->next;
				}

				sorted_linked_list_free(&neighbors);

				*(cd.vertex_neighbor_communities + i) = number_of_neighbor_communities;
			} else
				neighbor_communities_bad_computation = 1;
		}

		if(neighbor_communities_bad_computation) {
			printf("Could not compute neighbor communities!\n");

			return ILLEGAL_MODULARITY_VALUE;
		}

		// -------------------------------- SEQUENTIAL SECTION

		// Start copying in the first free position
		base = 0;
		total_exchanges = 0;
		do {
			total_exchanges += *(cd.vertex_neighbor_communities+base);
			base++;
		}while(base < dwg->size && *(cd.cumulative_edge_number+base-1) == total_exchanges);

		for(i = base; i < dwg->size; i++)
			for(j = 0; j < *(cd.vertex_neighbor_communities + i); j++) {
				*(cd.exchange_ranking + total_exchanges) = *(cd.exchange_ranking + *(cd.cumulative_edge_number + i - 1) + j);

				total_exchanges++;
			}

		// total_exchanges now represents the total number of active exchanges to be sorted

		// -------------------------------- END OF SEQUENTIAL SECTION

		if(total_exchanges > 0) {

			// Do the parallel sorting of the community pairings array

			if(!community_exchange_parallel_quick_sort_main(cd.exchange_ranking, total_exchanges,&sorted_output_multi_thread)){
				printf("Couldn't sort exchange pairings!");

				return ILLEGAL_MODULARITY_VALUE;
			}

			if(sorted_output_multi_thread)
				sorted_output_multi_thread_needs_free = 1;
			else
				sorted_output_multi_thread = cd.exchange_ranking;

			// -------------------------------- SEQUENTIAL SECTION

			// Do the sequential selection of the community pairings (iterate sequentially over the sorted ranking edges)

			if(!sequential_select_pairings(&cd, sorted_output_multi_thread, total_exchanges, &selected, &stop_scanning_position)) {
				printf("Couldn't select exchange pairings!");

				return ILLEGAL_MODULARITY_VALUE;
			}

			// -------------------------------- END OF SEQUENTIAL SECTION

			// Parallel updates for the communities

			// TODO Parallel for
//#pragma omp parallel for schedule(dynamic,EXCHANGE_CHUNK_SIZE) default(shared) private(i)
			for(i = 0; i < total_exchanges; i++)
				if(*(selected + i))
					apply_transfer_weighted(dwg,&cd,sorted_output_multi_thread+i);

			// Free memory used in the last computations
			free(selected);
			if(sorted_output_multi_thread_needs_free)
				free(sorted_output_multi_thread);

			// TODO End of substitute

			final_iteration_modularity = compute_modularity_weighted(dwg, &cd);

		} else
			final_iteration_modularity = initial_iteration_modularity;

		printf("\n\nEnd of Iteration #%d - Result:\n\n", phase_iteration_counter);
		community_developer_print(&cd,0);

		phase_iteration_counter++;

		printf("Final iteration modularity: %f. Modularity gain: %f\n", final_iteration_modularity, final_iteration_modularity - initial_iteration_modularity);

	} while (final_iteration_modularity - initial_iteration_modularity > minimum_improvement);

	output_translator_weighted(dwg, &cd, community_graph, community_vector);

	community_developer_free(&cd);

	final_phase_modularity = final_iteration_modularity;

	// New communities graph and results should be passed
	// TODO

	printf("Final phase modularity: %f. Modularity gain: %f\n", final_phase_modularity, final_phase_modularity - initial_phase_modularity);

	return final_phase_modularity - initial_phase_modularity;
}