// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// initFactories
void initFactories(std::string data_folder);
RcppExport SEXP _AdaptIG_initFactories(SEXP data_folderSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type data_folder(data_folderSEXP);
    initFactories(data_folder);
    return R_NilValue;
END_RCPP
}
// solveFSP
List solveFSP(std::string mh, Rcpp::CharacterVector rproblem, long seed, Rcpp::CharacterVector rparams, bool verbose);
RcppExport SEXP _AdaptIG_solveFSP(SEXP mhSEXP, SEXP rproblemSEXP, SEXP seedSEXP, SEXP rparamsSEXP, SEXP verboseSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type mh(mhSEXP);
    Rcpp::traits::input_parameter< Rcpp::CharacterVector >::type rproblem(rproblemSEXP);
    Rcpp::traits::input_parameter< long >::type seed(seedSEXP);
    Rcpp::traits::input_parameter< Rcpp::CharacterVector >::type rparams(rparamsSEXP);
    Rcpp::traits::input_parameter< bool >::type verbose(verboseSEXP);
    rcpp_result_gen = Rcpp::wrap(solveFSP(mh, rproblem, seed, rparams, verbose));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_AdaptIG_initFactories", (DL_FUNC) &_AdaptIG_initFactories, 1},
    {"_AdaptIG_solveFSP", (DL_FUNC) &_AdaptIG_solveFSP, 5},
    {NULL, NULL, 0}
};

RcppExport void R_init_AdaptIG(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}