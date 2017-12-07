
#include <AMReX_MLNodeLinOp.H>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace amrex {

MLNodeLinOp::MLNodeLinOp ()
{
    m_ixtype = IntVect::TheNodeVector();
}

MLNodeLinOp::~MLNodeLinOp () {}

void
MLNodeLinOp::define (const Vector<Geometry>& a_geom,
                     const Vector<BoxArray>& a_grids,
                     const Vector<DistributionMapping>& a_dmap,
                     const LPInfo& a_info)
{
    MLLinOp::define(a_geom, a_grids, a_dmap, a_info);
}

void
MLNodeLinOp::solutionResidual (int amrlev, MultiFab& resid, MultiFab& x, const MultiFab& b,
                               const MultiFab* crse_bcdata)
{
    AMREX_ALWAYS_ASSERT_WITH_MESSAGE(crse_bcdata == nullptr, "solutionResidual not fully implemented");
    AMREX_ALWAYS_ASSERT_WITH_MESSAGE(amrlev == 0, "solutionResidual not fully implemented");

    const int mglev = 0;
    apply(amrlev, mglev, resid, x, BCMode::Inhomogeneous);
    MultiFab::Xpay(resid, -1.0, b, 0, 0, 1, 0);
}

void
MLNodeLinOp::correctionResidual (int amrlev, int mglev, MultiFab& resid, MultiFab& x, const MultiFab& b,
                                 BCMode bc_mode, const MultiFab* crse_bcdata)
{
    AMREX_ALWAYS_ASSERT_WITH_MESSAGE(crse_bcdata == nullptr, "correctionResidual not fully implemented");
    AMREX_ALWAYS_ASSERT_WITH_MESSAGE(amrlev == 0, "correctionResidual not fully implemented");
    apply(amrlev, mglev, resid, x, BCMode::Homogeneous);
    MultiFab::Xpay(resid, -1.0, b, 0, 0, 1, 0);
}

void
MLNodeLinOp::apply (int amrlev, int mglev, MultiFab& out, MultiFab& in, BCMode bc_mode,
                    const MLMGBndry*) const
{
    applyBC(amrlev, mglev, in);
    Fapply(amrlev, mglev, out, in);
}

void
MLNodeLinOp::smooth (int amrlev, int mglev, MultiFab& sol, const MultiFab& rhs,
                     bool skip_fillboundary) const
{
    if (!skip_fillboundary) {
        applyBC(amrlev, mglev, sol);
    }
    Fsmooth(amrlev, mglev, sol, rhs);
}

}

