#include "Solver.h"
#include "SolverTypes.h"

using namespace Minisat;

extern "C" {

#include "csolver.h"

CSolver*
csolver_init(){
	CSolver* s = (CSolver*) new Solver ();
	return s;
}

CSolver*
csolver_reset(CSolver * s){
	delete (Solver *) s;
	return csolver_init();
}

void
csolver_release(CSolver * s){
	delete (Solver *) s;
}

int
csolver_solve(CSolver * s){
	if(((Solver *) s)->solve()) return 1;
	return 0;
}

void
csolver_clause(CSolver * s, int nLits, ...){
	Solver * w = (Solver *) s;
	int lit, var;
	vec<Lit> lit_vec;

	va_list args;
	va_start(args, nLits);

	for(int i = 0; i < nLits; ++i){
		lit = va_arg(args, int);
		if (!lit) return;
		var = abs(lit) - 1;
        while (var >= w->nVars()) w->newVar();
        lit_vec.push((lit > 0) ? mkLit(var) : ~mkLit(var));
	}

	w->addClause_(lit_vec);

	va_end(args);
}

void
csolver_clausezt(CSolver * s, ...){
	Solver * w = (Solver *) s;
	int lit, var;
	vec<Lit> lit_vec;

	va_list args;
	va_start(args, s);

	while((lit = va_arg(args, int))){
		var = abs(lit) - 1;
        while (var >= w->nVars()) w->newVar();
        lit_vec.push((lit > 0) ? mkLit(var) : ~mkLit(var));
	}

	w->addClause_(lit_vec);

	va_end(args);
}

void
csolver_aclause(CSolver * s, int nLits, int * lits){
	Solver * w = (Solver *) s;
	vec<Lit> lit_vec;
	int var;
	for(int i = 0; i < nLits; ++i){
		if(!lits[i]) return;
		var = abs(lits[i]) - 1;
        while (var >= w->nVars()) w->newVar();
        lit_vec.push((lits[i] > 0) ? mkLit(var) : ~mkLit(var));
	}

	w->addClause_(lit_vec);

}
void
csolver_aclausezt(CSolver * s, int * lits){
	Solver * w = (Solver *) s;
	vec<Lit> lit_vec;
	int var;
	while(*lits){
		var = abs(*lits) - 1;
        while (var >= w->nVars()) w->newVar();
        lit_vec.push((*lits > 0) ? mkLit(var) : ~mkLit(var));
		lits += 1;
	}

	w->addClause_(lit_vec);
}

int
csolver_val(CSolver * s, int lit){
	Solver * w = (Solver *) s;
	return toInt(w->modelValue((Var) (lit - 1))) ? -1 : 1;
}

const struct clibrary LibCSolver = {
	.init =			csolver_init,
	.reset =		csolver_reset,
	.release =		csolver_release,
	.solve =		csolver_solve,

	.clause = 		csolver_clause,
	.clausezt =		csolver_clausezt,
	.aclause =		csolver_aclause,
	.aclausezt =	csolver_aclausezt,

	.val =			csolver_val
};
}
