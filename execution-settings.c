#include "execution-settings.h"

char * file_format_name(int id){
	switch(id) {
	case FILE_FORMAT_EDGE_LIST_NOT_WEIGHTED:
		return FILE_FORMAT_EDGE_LIST_NOT_WEIGHTED_NAME;
	case FILE_FORMAT_EDGE_LIST_WEIGHTED:
		return FILE_FORMAT_EDGE_LIST_WEIGHTED_NAME;
	case FILE_FORMAT_METIS:
		return FILE_FORMAT_METIS_NAME;
	default:
		return FILE_FORMAT_INVALID_NAME;
	}
}

int algorithm_version_parallel(int id){
	switch(id) {
	case ALGORITHM_VERSION_SEQUENTIAL_0:
		return 0;
	case ALGORITHM_VERSION_PARALLEL_1_TRANSFER_SORT_SELECT:
		return 1;
	default:
		return -1;
	}
}

char * algorithm_version_name(int id){
	switch(id) {
	case ALGORITHM_VERSION_SEQUENTIAL_0:
		return ALGORITHM_VERSION_SEQUENTIAL_0_NAME;
	case ALGORITHM_VERSION_PARALLEL_1_TRANSFER_SORT_SELECT:
		return ALGORITHM_VERSION_PARALLEL_1_TRANSFER_SORT_SELECT_NAME;
	default:
		return ALGORITHM_VERSION_INVALID_NAME;
	}
}
