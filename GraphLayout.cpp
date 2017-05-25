//#include "GraphLayout.h"
//
///**
//* \ingroup layout
//* \function igraph_layout_kamada_kawai
//* \brief Places the vertices on a plane according the Kamada-Kawai algorithm.
//*
//* </para><para>
//* This is a force directed layout, see  Kamada, T. and Kawai, S.: An
//* Algorithm for Drawing General Undirected Graphs. Information
//* Processing Letters, 31/1, 7--15, 1989.
//* \param graph A graph object.
//* \param res Pointer to an initialized matrix object. This will
//*        contain the result (x-positions in column zero and
//*        y-positions in column one) and will be resized if needed.
//* \param use_seed Boolean, whether to use the values supplied in the
//*        \p res argument as the initial configuration. If zero then a
//*        random initial configuration is used.
//* \param maxiter The maximum number of iterations to perform. A reasonable
//*        default value is at least ten (or more) times the number of
//*        vertices.
//* \param epsilon Stop the iteration, if the maximum delta value of the
//*        algorithm is smaller than still. It is safe to leave it at zero,
//*        and then \p maxiter iterations are performed.
//* \param kkconst The Kamada-Kawai vertex attraction constant.
//*        Typical value: number of vertices.
//* \param weights Edge weights, larger values will result longer edges.
//* \param minx Pointer to a vector, or a \c NULL pointer. If not a
//*        \c NULL pointer then the vector gives the minimum
//*        \quote x \endquote coordinate for every vertex.
//* \param maxx Same as \p minx, but the maximum \quote x \endquote
//*        coordinates.
//* \param miny Pointer to a vector, or a \c NULL pointer. If not a
//*        \c NULL pointer then the vector gives the minimum
//*        \quote y \endquote coordinate for every vertex.
//* \param maxy Same as \p miny, but the maximum \quote y \endquote
//*        coordinates.
//* \return Error code.
//*
//* Time complexity: O(|V|) for each iteration, after an O(|V|^2
//* log|V|) initialization step. |V| is the number of vertices in the
//* graph.
//*/
//
//int GraphLayout::igraph_layout_kamada_kawai( igraph_t *graph, igraph_matrix_t *res,
//	igraph_bool_t use_seed, igraph_integer_t maxiter,
//	igraph_real_t epsilon, igraph_real_t kkconst,
//	igraph_vector_t *weights,
//	igraph_vector_t *minx,igraph_vector_t *maxx,
//	igraph_vector_t *miny, igraph_vector_t *maxy) {
//
//	igraph_integer_t no_nodes = igraph_vcount(graph);
//	igraph_integer_t no_edges = igraph_ecount(graph);
//	igraph_real_t L, L0 = sqrt(no_nodes);
//	igraph_matrix_t dij, lij, kij;
//	igraph_real_t max_dij;
//	igraph_vector_t D1, D2;
//	igraph_integer_t i, j, m;
//
//	const char* error;
//
//	if (maxiter < 0) {
//		error = "Number of iterations must be non-negatice in "
//			"Kamada-Kawai layout";
//	}
//	if (kkconst <= 0) {
//		error = "`K' constant must be positive in Kamada-Kawai layout";
//	}
//
//	if (use_seed && (igraph_matrix_nrow(res) != no_nodes ||
//		igraph_matrix_ncol(res) != 2)) {
//		error = "Invalid start position matrix size in "
//			"Kamada-Kawai layout";
//	}
//	if (weights && igraph_vector_size(weights) != no_edges) {
//		error = "Invalid weight vector length";
//	}
//
//	if (minx && igraph_vector_size(minx) != no_nodes) {
//		error = "Invalid minx vector length";
//	}
//	if (maxx && igraph_vector_size(maxx) != no_nodes) {
//		error = "Invalid maxx vector length";
//	}
//	if (minx && maxx && !igraph_vector_all_le(minx, maxx)) {
//		error = "minx must not be greater than maxx";
//	}
//	if (miny && igraph_vector_size(miny) != no_nodes) {
//		error = "Invalid miny vector length";
//	}
//	if (maxy && igraph_vector_size(maxy) != no_nodes) {
//		error = "Invalid maxy vector length";
//	}
//	if (miny && maxy && !igraph_vector_all_le(miny, maxy)) {
//		error = "miny must not be greater than maxy";
//	}
//
//	if (!use_seed) {
//		if (minx || maxx || miny || maxy) {
//			const igraph_real_t width = sqrt(no_nodes), height = width;
//			IGRAPH_CHECK(igraph_matrix_resize(res, no_nodes, 2));
//			RNG_BEGIN();
//			for (i = 0; i<no_nodes; i++) {
//				igraph_real_t x1 = minx ? VECTOR(*minx)[i] : -width / 2;
//				igraph_real_t x2 = maxx ? VECTOR(*maxx)[i] : width / 2;
//				igraph_real_t y1 = miny ? VECTOR(*miny)[i] : -height / 2;
//				igraph_real_t y2 = maxy ? VECTOR(*maxy)[i] : height / 2;
//				if (!igraph_finite(x1)) { x1 = -width / 2; }
//				if (!igraph_finite(x2)) { x2 = width / 2; }
//				if (!igraph_finite(y1)) { y1 = -height / 2; }
//				if (!igraph_finite(y2)) { y2 = height / 2; }
//				MATRIX(*res, i, 0) = RNG_UNIF(x1, x2);
//				MATRIX(*res, i, 1) = RNG_UNIF(y1, y2);
//			}
//			RNG_END();
//		}
//		else {
//			igraph_layout_circle(graph, res);
//		}
//	}
//
//	if (no_nodes <= 1) { return 0; }
//
//	IGRAPH_MATRIX_INIT_FINALLY(&dij, no_nodes, no_nodes);
//	IGRAPH_MATRIX_INIT_FINALLY(&kij, no_nodes, no_nodes);
//	IGRAPH_MATRIX_INIT_FINALLY(&lij, no_nodes, no_nodes);
//
//	if (weights && igraph_vector_min(weights) < 0) {
//		IGRAPH_CHECK(igraph_shortest_paths_bellman_ford(graph, &dij, igraph_vss_all(),
//			igraph_vss_all(), weights,
//			IGRAPH_ALL));
//	}
//	else {
//
//		IGRAPH_CHECK(igraph_shortest_paths_dijkstra(graph, &dij, igraph_vss_all(),
//			igraph_vss_all(), weights,
//			IGRAPH_ALL));
//	}
//
//	max_dij = 0.0;
//	for (i = 0; i<no_nodes; i++) {
//		for (j = i + 1; j<no_nodes; j++) {
//			if (!igraph_finite(MATRIX(dij, i, j))) { continue; }
//			if (MATRIX(dij, i, j) > max_dij) { max_dij = MATRIX(dij, i, j); }
//		}
//	}
//	for (i = 0; i<no_nodes; i++) {
//		for (j = 0; j<no_nodes; j++) {
//			if (MATRIX(dij, i, j) > max_dij) { MATRIX(dij, i, j) = max_dij; }
//		}
//	}
//
//	L = L0 / max_dij;
//	for (i = 0; i<no_nodes; i++) {
//		for (j = 0; j<no_nodes; j++) {
//			igraph_real_t tmp = MATRIX(dij, i, j) * MATRIX(dij, i, j);
//			if (i == j) { continue; }
//			MATRIX(kij, i, j) = kkconst / tmp;
//			MATRIX(lij, i, j) = L * MATRIX(dij, i, j);
//		}
//	}
//
//	/* Initialize delta */
//	IGRAPH_VECTOR_INIT_FINALLY(&D1, no_nodes);
//	IGRAPH_VECTOR_INIT_FINALLY(&D2, no_nodes);
//	for (m = 0; m<no_nodes; m++) {
//		igraph_real_t myD1 = 0.0, myD2 = 0.0;
//		for (i = 0; i<no_nodes; i++) {
//			igraph_real_t dx, dy, mi_dist;
//			if (i == m) { continue; }
//			dx = MATRIX(*res, m, 0) - MATRIX(*res, i, 0);
//			dy = MATRIX(*res, m, 1) - MATRIX(*res, i, 1);
//			mi_dist = sqrt(dx * dx + dy * dy);
//			myD1 += MATRIX(kij, m, i) * (dx - MATRIX(lij, m, i) * dx / mi_dist);
//			myD2 += MATRIX(kij, m, i) * (dy - MATRIX(lij, m, i) * dy / mi_dist);
//		}
//		VECTOR(D1)[m] = myD1;
//		VECTOR(D2)[m] = myD2;
//	}
//
//	for (j = 0; j<maxiter; j++) {
//		igraph_real_t myD1, myD2, A, B, C;
//		igraph_real_t max_delta, delta_x, delta_y;
//		igraph_real_t old_x, old_y, new_x, new_y;
//
//		myD1 = 0.0, myD2 = 0.0, A = 0.0, B = 0.0, C = 0.0;
//
//		/* Select maximal delta */
//		m = 0; max_delta = -1;
//		for (i = 0; i<no_nodes; i++) {
//			igraph_real_t delta = (VECTOR(D1)[i] * VECTOR(D1)[i] +
//				VECTOR(D2)[i] * VECTOR(D2)[i]);
//			if (delta > max_delta) {
//				m = i; max_delta = delta;
//			}
//		}
//		if (max_delta < epsilon) { break; }
//		old_x = MATRIX(*res, m, 0);
//		old_y = MATRIX(*res, m, 1);
//
//		/* Calculate D1 and D2, A, B, C */
//		for (i = 0; i<no_nodes; i++) {
//			igraph_real_t dx, dy, dist, den;
//			if (i == m) { continue; }
//			dx = old_x - MATRIX(*res, i, 0);
//			dy = old_y - MATRIX(*res, i, 1);
//			dist = sqrt(dx * dx + dy * dy);
//			den = dist * (dx * dx + dy * dy);
//			A += MATRIX(kij, m, i) * (1 - MATRIX(lij, m, i) * dy * dy / den);
//			B += MATRIX(kij, m, i) * MATRIX(lij, m, i) * dx * dy / den;
//			C += MATRIX(kij, m, i) * (1 - MATRIX(lij, m, i) * dx * dx / den);
//		}
//		myD1 = VECTOR(D1)[m];
//		myD2 = VECTOR(D2)[m];
//
//		/* Need to solve some linear equations */
//		delta_y = (B * myD1 - myD2 * A) / (C * A - B * B);
//		delta_x = -(myD1 + B * delta_y) / A;
//
//		new_x = old_x + delta_x;
//		new_y = old_y + delta_y;
//
//		/* Limits, if given */
//		if (minx && new_x < VECTOR(*minx)[m]) { new_x = VECTOR(*minx)[m]; }
//		if (maxx && new_x > VECTOR(*maxx)[m]) { new_x = VECTOR(*maxx)[m]; }
//		if (miny && new_y < VECTOR(*miny)[m]) { new_y = VECTOR(*miny)[m]; }
//		if (maxy && new_y > VECTOR(*maxy)[m]) { new_y = VECTOR(*maxy)[m]; }
//
//		/* Update delta, only with/for the affected node */
//		VECTOR(D1)[m] = VECTOR(D2)[m] = 0.0;
//		for (i = 0; i<no_nodes; i++) {
//			igraph_real_t old_dx, old_dy, old_mi, new_dx, new_dy, new_mi_dist, old_mi_dist;
//			if (i == m) { continue; }
//			old_dx = old_x - MATRIX(*res, i, 0);
//			old_dy = old_y - MATRIX(*res, i, 1);
//			old_mi_dist = sqrt(old_dx * old_dx + old_dy * old_dy);
//			new_dx = new_x - MATRIX(*res, i, 0);
//			new_dy = new_y - MATRIX(*res, i, 1);
//			new_mi_dist = sqrt(new_dx * new_dx + new_dy * new_dy);
//
//			VECTOR(D1)[i] -= MATRIX(kij, m, i) *
//				(-old_dx + MATRIX(lij, m, i) * old_dx / old_mi_dist);
//			VECTOR(D2)[i] -= MATRIX(kij, m, i) *
//				(-old_dy + MATRIX(lij, m, i) * old_dy / old_mi_dist);
//			VECTOR(D1)[i] += MATRIX(kij, m, i) *
//				(-new_dx + MATRIX(lij, m, i) * new_dx / new_mi_dist);
//			VECTOR(D2)[i] += MATRIX(kij, m, i) *
//				(-new_dy + MATRIX(lij, m, i) * new_dy / new_mi_dist);
//
//			VECTOR(D1)[m] += MATRIX(kij, m, i) *
//				(new_dx - MATRIX(lij, m, i) * new_dx / new_mi_dist);
//			VECTOR(D2)[m] += MATRIX(kij, m, i) *
//				(new_dy - MATRIX(lij, m, i) * new_dy / new_mi_dist);
//		}
//
//		/* Update coordinates*/
//		MATRIX(*res, m, 0) = new_x;
//		MATRIX(*res, m, 1) = new_y;
//	}
//
//	igraph_vector_destroy(&D2);
//	igraph_vector_destroy(&D1);
//	igraph_matrix_destroy(&lij);
//	igraph_matrix_destroy(&kij);
//	igraph_matrix_destroy(&dij);
//	IGRAPH_FINALLY_CLEAN(5);
//
//	return 0;
//}