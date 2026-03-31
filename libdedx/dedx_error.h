#ifndef DEDX_ERROR_H
#define DEDX_ERROR_H

/**
 * @file dedx_error.h
 * @brief Error codes returned via the int *err output parameter.
 *
 * Functions signal success by setting `*err = DEDX_OK`.
 * On failure they set `*err` to one of the values below and, unless
 * stated otherwise, return 0 / NULL / -1.
 */

/** No error. */
#define DEDX_OK 0

/** @defgroup err_file File I/O errors (1–10)
 * @{ */
#define DEDX_ERR_NO_COMPOS_FILE 1   /**< compos.txt not found */
#define DEDX_ERR_NO_GAS_FILE 2      /**< gas_states.dat not found */
#define DEDX_ERR_NO_CHARGE_FILE 3   /**< effective_charge.dat not found */
#define DEDX_ERR_NO_BINARY_DATA 4   /**< binary stopping-power data file inaccessible */
#define DEDX_ERR_NO_BINARY_ENERGY 5 /**< binary energy-grid file inaccessible */
#define DEDX_ERR_WRITE_FAILED 6     /**< cannot write to disk */
#define DEDX_ERR_NO_ENERGY_FILE 7   /**< energy .dat source file unreadable */
#define DEDX_ERR_NO_DATA_FILE 8     /**< stopping-power .dat source file unreadable */
#define DEDX_ERR_NO_NAMES_FILE 9    /**< short_names file unreadable (reserved internal code) */
#define DEDX_ERR_NO_COMPOSITION 10  /**< elemental composition file unreadable */
/** @} */

/** @defgroup err_bounds Bounds errors (101)
 * @{ */
#define DEDX_ERR_ENERGY_OUT_OF_RANGE 101 /**< requested energy outside tabulated range */
/** @} */

/** @defgroup err_data Data and configuration errors (201–210)
 * @{ */
#define DEDX_ERR_TARGET_NOT_FOUND 201        /**< target not found in composition table */
#define DEDX_ERR_COMBINATION_NOT_FOUND 202   /**< ion/target combination not in data file */
#define DEDX_ERR_INVALID_DATASET_ID 203      /**< dataset ID does not exist in workspace */
#define DEDX_ERR_NOT_AN_ELEMENT 204          /**< ID does not correspond to an atomic element */
#define DEDX_ERR_ESTAR_NOT_IMPL 205          /**< ESTAR program is not implemented */
#define DEDX_ERR_ION_NOT_SUPPORTED_MSTAR 206 /**< ion not supported for MSTAR (reserved legacy code) */
#define DEDX_ERR_ION_NOT_SUPPORTED 207       /**< ion not supported by requested program */
#define DEDX_ERR_RHO_REQUIRED 208            /**< target density rho must be provided */
#define DEDX_ERR_ION_A_REQUIRED 209          /**< nucleon number ion_a must be provided */
#define DEDX_ERR_INVALID_I_VALUE 210         /**< mean excitation potential must be > 0 */
#define DEDX_ERR_INCONSISTENT_COMPOUND 211   /**< inconsistent compound specification */
/** @} */

/** @defgroup err_memory Memory errors (301)
 * @{ */
#define DEDX_ERR_NO_MEMORY 301 /**< memory allocation failed */
/** @} */

#endif /* DEDX_ERROR_H */
