#pragma once
#define CELL(m,r,c) (((m)->data)[((m)->n)*(r) + (c)])
#define MIN_WORK_LEVELS 5
#define WORK_SLICE 10000000
struct prefix_list_node {
	struct prefix_list_node *next;
	int prefix_len;
	int prefix[];
};
struct termination_state {
	MPI_Request request; /* Make sure we don't modify the token while it's still being transmitted
						 */
	int token; /* whether I have the token, and what color it is */
	int mycolor; /* what color I am */
};
struct tsp_state {
	int num_procs;
	int myrank;
	int cost; /* the cost of this tour */
	int ub; /* the shortest path found so far (globally) */
	int ub_rank;
	int give_depth; /* the minimum depth from which we can give away work */
	int subtree_depth;
	int *last_started; /* an array of the last started node values at a certain depth */
	int len; /* length of the tour so far */
	struct tsp_matrix *matrix; /* the adjacency matrix of our TSP graph */
	struct termination_state term_token;
	int work_state;
	int work_partner;
	int *best_tour;
	int *send_buf;
	MPI_Request work_req;
	double work_time;
	double total_time;
	int tour[]; /* an array of node numbers, in the order which they are visited */
				/* tour must be the last field, since it's variable size! */
};
#define NO_TOKEN 0
#define WHITE 1
#define BLACK 2
#define NEED_WORK 0
#define WORKING 1
#define IDLE 2
#define QUIT 3
#define WORK_REQ_PENDING 4
struct tsp_state *tsp_state_alloc(struct tsp_matrix *matrix);
#define UB_TAG 1
#define WORK_REQ_TAG 2
#define WORK_ACK_TAG 3
#define TOKEN_TAG 4
#define TERMINATION_TAG 5
#define BEST_PATH_TAG 6
#define BEST_PATH_REQ_TAG 7
	void tsp(struct tsp_state *state);
	  void do_work(struct tsp_state *state);
	  void service_work_request(struct tsp_state *state, MPI_Status status);
	  void service_pending_messages(struct tsp_state *state);
	  void request_work(struct tsp_state *state);
	  void send_ub_message(struct tsp_state *state);
	  void send_token(struct tsp_state *state);