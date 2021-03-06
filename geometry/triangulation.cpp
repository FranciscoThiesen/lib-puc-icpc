#include <map>

typedef pair<int, int> edge;

struct triangulation: public map<edge, int> {
	edge sym(edge e)   { return edge(e.second, e.first); }
	edge lnext(edge e) { return edge(e.second, (*this)[e]); }
	edge lprev(edge e) { return edge((*this)[e], e.first); }
	edge dnext(edge e) { return lprev(sym(lprev(e))); }
	edge dprev(edge e) { return lnext(sym(lnext(e))); }

	void new_tri(edge e, int r) {
		if (count(e)) { erase(lnext(e)); erase(lprev(e)); }
		(*this)[e] = r; (*this)[lnext(e)] = e.first; (*this)[lprev(e)] = e.second;
	}

	// Digite esta função apenas para triangulações com restrições.
	void bdsm(field& F, edge e) {
		int a, b, c, d, org = e.first, dest = e.second, topo = 0;
		edge xt;
		edge pilha[TAM];
		if (count(e)) return;
		for (iterator it = lower_bound(edge(org, 0)); ; it++) {
			xt = lnext(it->first); a = xt.first, b = xt.second;
			if (b < 0) continue;
			if (seg_intersect(F.v[a], F.v[b], F.v[org], F.v[dest])) break;
		}
		while (xt.second != dest) {
			pilha[topo++] = xt; xt = sym(xt);
			xt = F.ccw(org, dest, (*this)[xt]) >= 0 ? lnext(xt) : lprev(xt);
			while (topo > 0) {
				edge ee = pilha[topo-1];
				a = ee.first; b = ee.second;
				c = (*this)[ee]; d = (*this)[sym(ee)];
				if (F.ccw(d, c, b) >= 0 || F.ccw(c, d, a) >= 0) break;
				erase(ee); erase(sym(ee)); xt = edge(d, c);
				new_tri(xt, a); new_tri(sym(xt), b);
				topo--;
				xt = F.ccw(org, dest, d) >= 0 ? lprev(xt) : lnext(sym(xt));
			}
		}
	}

	void triangulate(field& F) {
		int J[TAM], i, k, topo = 0;
		edge pilha[TAM];
		clear();
		for (int i = 0; i < F.n; i++) J[i] = i;
		sort(J, J + F.n, make_index_lt(F.v));
		for (i = 2; i < F.n; i++) if (k = F.ccw(J[0], J[1], J[i])) break;
		if (i >= F.n) return;
		for (int j = 1; j < i; j++) {
			edge e(J[j-1], J[j]);
			new_tri(e, (k > 0) ? J[i] : -1);
			new_tri(sym(e), (k > 0) ? -1 : J[i]);
		}
		edge lb(J[i], J[(k > 0) ? i-1 : 0]), ub(J[(k > 0) ? 0 : i-1], J[i]);
		for (i++; i < F.n; i++) {
			while (F.ccw(lb.first, lb.second, J[i]) >= 0) lb = dprev(lb);
			while (F.ccw(ub.first, ub.second, J[i]) >= 0) ub = dnext(ub);
			for (edge e = dnext(lb); e != ub; e = dnext(e)) pilha[topo++] = e;
			while (topo > 0) new_tri(pilha[--topo], J[i]);
			edge e(-1, J[i]);
			new_tri(e, lb.first); new_tri(sym(e), ub.second);
			lb = lnext(e); ub = dnext(lb);
		}
		// Digite esta linha somente para triangulações com restrições.
		for (i = 0; i < F.m; i++) {
			bdsm(F, edge(F.b[i].first, F.b[i].second));
		}
	}
};
