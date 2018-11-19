struct tsp_matrix {
	int n;
	int *data;
	char name[100];
};
void parse_matrix_from_file(struct tsp_matrix *matrix, FILE *file);
void parse_geo_function(struct tsp_matrix *matrix, FILE *file);
void parse_explicit_lowerdiagrow(struct tsp_matrix *matrix, FILE *file);
void print_matrix(struct tsp_matrix *m);
