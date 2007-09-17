#ifndef included_LagM3DDataWriter
#define included_LagM3DDataWriter

// Filename: LagM3DDataWriter.h
// Last modified: <17.Sep.2007 13:34:40 griffith@box221.cims.nyu.edu>
// Created on 11 Sep 2007 by Boyce Griffith (griffith@box221.cims.nyu.edu)

/////////////////////////////// INCLUDES /////////////////////////////////////

// IBAMR INCLUDES
#include <ibamr/LNodeLevelData.h>

// SAMRAI INCLUDES
#include <IntVector.h>
#include <PatchHierarchy.h>
#include <tbox/DescribedClass.h>
#include <tbox/Pointer.h>

// PETSC INCLUDES
#include <petscvec.h>
#include <petscao.h>

// C++ STDLIB INCLUDES
#include <map>
#include <set>
#include <vector>

/////////////////////////////// CLASS DEFINITION /////////////////////////////

namespace IBAMR
{
/*!
 * \brief Class LagM3DDataWriter provides functionality to output Lagrangian
 * data for visualization via myocardial3D (referred to as m3D throughout the
 * class documentation).
 */
class LagM3DDataWriter
    : public virtual SAMRAI::tbox::DescribedClass
{
public:
    /*!
     * \brief Constructor.
     *
     * \param object_name          String used for error reporting.
     * \param dump_directory_name  String indicating the directory where visualization data is to be written.
     */
    LagM3DDataWriter(
        const std::string& object_name,
        const std::string& dump_directory_name);

    /*!
     * \brief Destructor.
     */
    ~LagM3DDataWriter();

    /*!
     * \name Methods to set the hierarchy and range of levels.
     */
    //\{

    /*!
     * \brief Reset the patch hierarchy over which operations occur.
     */
    void
    setPatchHierarchy(
        SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > hierarchy);

    /*!
     * \brief Reset range of patch levels over which operations occur.
     */
    void
    resetLevels(
        const int coarsest_ln,
        const int finest_ln);

    //\}

    /*!
     * \brief Register the patch data descriptor index corresponding to the
     * SAMRAI::pdat::IndexData containing the marker data to be visualized.
     */
    void
    registerIBMarkerPatchDataIndex(
        const int mark_idx);

    /*!
     * \brief Register a range of marker indices that are to be visualized as a
     * cloud of marker particles.
     *
     * \note This method is not collective over all MPI processes.  A particular
     * cloud of markers must be registered on only \em one MPI process.
     */
    void
    registerMarkerCloud(
        const std::string& name,
        const int nmarks,
        const int first_marker_idx);

    /*!
     * \brief Register a range of Lagrangian indices that are to be treated as a
     * logically Cartesian block.
     *
     * \note This method is not collective over all MPI processes.  A particular
     * block of indices must be registered on only \em one MPI process.
     */
    void
    registerLogicallyCartesianBlock(
        const std::string& name,
        const SAMRAI::hier::IntVector<NDIM>& nelem,
        const SAMRAI::hier::IntVector<NDIM>& periodic,
        const int first_lag_idx,
        const int level_number);

    /*!
     * \brief Register the coordinates of the curvilinear mesh with the m3D data
     * writer.
     */
    void
    registerCoordsData(
        SAMRAI::tbox::Pointer<LNodeLevelData> coords_data,
        const int level_number);

    /*!
     * \brief Register a single Lagrangian AO (application ordering) objects
     * with the m3D data writer.
     *
     * These AO objects are used to map between (fixed) Lagrangian indices and
     * (time-dependent) PETSc indices.  Each time that the AO objects are reset
     * (e.g., during adaptive regridding), the new AO objects must be supplied
     * to the m3D data writer.
     */
    void
    registerLagrangianAO(
        AO& ao,
        const int level_number);

    /*!
     * \brief Register a collection of Lagrangian AO (application ordering)
     * objects with the m3D data writer.
     *
     * These AO objects are used to map between (fixed) Lagrangian indices and
     * (time-dependent) PETSc indices.  Each time that the AO objects are reset
     * (e.g., during adaptive regridding), the new AO objects must be supplied
     * to the m3D data writer.
     */
    void
    registerLagrangianAO(
        std::vector<AO>& ao,
        const int coarsest_ln,
        const int finest_ln);

    /*!
     * \brief Write the plot data to disk.
     */
    void
    writePlotData(
        const int time_step_number,
        const double simulation_time);

protected:

private:
    /*!
     * \brief Default constructor.
     *
     * \note This constructor is not implemented and should not be used.
     */
    LagM3DDataWriter();

    /*!
     * \brief Copy constructor.
     *
     * \note This constructor is not implemented and should not be used.
     *
     * \param from The value to copy to this object.
     */
    LagM3DDataWriter(
        const LagM3DDataWriter& from);

    /*!
     * \brief Assignment operator.
     *
     * \note This operator is not implemented and should not be used.
     *
     * \param that The value to assign to this object.
     *
     * \return A reference to this object.
     */
    LagM3DDataWriter&
    operator=(
        const LagM3DDataWriter& that);

    /*!
     * \brief Build the VecScatter objects required to communicate data for
     * plotting.
     */
    void
    buildVecScatters(
        AO& ao,
        const int level_number);

    /*
     * The object name is used for error reporting purposes.
     */
    std::string d_object_name;

    /*
     * The directory where data is to be dumped.
     */
    std::string d_dump_directory_name;

    /*
     * Time step number (passed in by user).
     */
    int d_time_step_number;

    /*
     * Grid hierarchy information.
     */
    SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > d_hierarchy;
    int d_coarsest_ln, d_finest_ln;

    /*
     * Information about the indices in the local marker clouds.
     */
    int d_mark_idx, d_nclouds;
    std::vector<std::string> d_cloud_names;
    std::vector<int> d_cloud_nmarks, d_cloud_first_mark_idx;

    /*
     * Information about the indices in the logically Cartesian subgrids.
     */
    std::vector<int> d_nblocks;
    std::vector<std::vector<std::string> > d_block_names;
    std::vector<std::vector<SAMRAI::hier::IntVector<NDIM> > > d_block_nelems;
    std::vector<std::vector<SAMRAI::hier::IntVector<NDIM> > > d_block_periodic;
    std::vector<std::vector<int> > d_block_nfibers, d_block_ngroups, d_block_first_lag_idx;

    /*
     * Coordinates and variable data for plotting.
     */
    std::vector<SAMRAI::tbox::Pointer<LNodeLevelData> > d_coords_data;

    std::vector<int> d_nvars;
    std::vector<std::vector<std::string > > d_var_names;
    std::vector<std::vector<int> > d_var_depths;
    std::vector<std::vector<SAMRAI::tbox::Pointer<LNodeLevelData> > > d_var_data;

    /*
     * Data for obtaining local data.
     */
    std::vector<AO> d_ao;
    std::vector<bool> d_build_vec_scatters;
    std::vector<std::map<int,Vec> > d_src_vec, d_dst_vec;
    std::vector<std::map<int,VecScatter> > d_vec_scatter;
};
}// namespace IBAMR

/////////////////////////////// INLINE ///////////////////////////////////////

//#include <ibamr/LagM3DDataWriter.I>

//////////////////////////////////////////////////////////////////////////////

#endif //#ifndef included_LagM3DDataWriter