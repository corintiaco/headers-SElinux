* edge cases. (e.g. V2 memory.swap.max accepts "max", but V1
* memory.memsw.limit_in_bytes does not).
*/

#ifndef TST_CGROUP_H
#define TST_CGROUP_H

#include <sys/types.h>

/* CGroups Kernel API version */
enum tst_cg_ver {
    TST_CG_V1 = 1,
    TST_CG_V2 = 2,
};

/* This value is greater than ROOTS_MAX in tst_cgroup.c. */
#define TST_CG_ROOTS_MAX 32

/* Used to specify CGroup hierarchy configuration options, allowing a
 * test to request a particular CGroup structure.
 */
struct tst_cg_opts {
    /* Call tst_brk with TCONF if the controller is not on this
     * version. Defautls to zero to accept any version.
     */
    enum tst_cg_ver needs_ver;
    /* Pass in a specific pid to create and identify the test
     * directory as opposed to the default pid of the calling process.
     */
    int test_pid;
    int needs_nsdelegate;
};

/* A Control Group in LTP's aggregated hierarchy */
struct tst_cg_group;

/* Populated with a reference to this tests's CGroup */
extern const struct tst_cg_group *const tst_cg;
extern const struct tst_cg_group *const tst_cg_drain;

/* Search the system for mounted cgroups and available
 * controllers. Called automatically by tst_cg_require.
 */
void tst_cg_scan(void);
/* Print the config detected by tst_cg_scan and print the internal
 * state associated with each controller. Output can be passed to
 * tst_cg_load_config to configure the internal state to that of the
 * config between program invocations.
 */
void tst_cg_print_config(void);

/* Load the config printed out by tst_cg_print_config and configure the internal
 * libary state to match the config. Used to allow tst_cg_cleanup to properly
 * cleanup mounts and directories created by tst_cg_require between program
 * invocations.
 */
void tst_cg_load_config(const char *const config);

/* Ensure the specified controller is available in the test's default
 * CGroup, mounting/enabling it if necessary. Usually this is not
 * necessary use tst_test.needs_cgroup_ctrls instead.
 */
void tst_cg_require(const char *const ctrl_name,
                    const struct tst_cg_opts *const options)
__attribute__ ((nonnull));

/* Tear down any CGroups created by calls to tst_cg_require */
void tst_cg_cleanup(void);

/* Call this in setup after you call tst_cg_require and want to
 * initialize tst_cg and tst_cg_drain. See tst_cg_require.
 */
void tst_cg_init(void);

/* Create a descendant CGroup */
struct tst_cg_group *
tst_cg_group_mk(const struct tst_cg_group *const parent,
                const char *const group_name_fmt, ...)
__attribute__ ((nonnull, warn_unused_result, format (printf, 2, 3)));

const char *
tst_cg_group_name(const struct tst_cg_group *const cg)
__attribute__ ((nonnull, warn_unused_result));

/* This call returns a fd pointing to a v2 directory */
int tst_cg_group_unified_dir_fd(const struct tst_cg_group *const cg)
__attribute__ ((nonnull, warn_unused_result));

/* Remove a descendant CGroup */
struct tst_cg_group *
tst_cg_group_rm(struct tst_cg_group *const cg)
__attribute__ ((nonnull, warn_unused_result));

#define TST_CG_VER(cg, ctrl_name) \
	tst_cg_ver(__FILE__, __LINE__, (cg), (ctrl_name))

enum tst_cg_ver tst_cg_ver(const char *const file, const int lineno,
                           const struct tst_cg_group *const cg,
                           const char *const ctrl_name)
__attribute__ ((nonnull, warn_unused_result));

#define TST_CG_VER_IS_V1(cg, ctrl_name) \
	(TST_CG_VER((cg), (ctrl_name)) == TST_CG_V1)

#define SAFE_CG_HAS(cg, file_name) \
	safe_cg_has(__FILE__, __LINE__, (cg), (file_name))

int safe_cg_has(const char *const file, const int lineno,
                const struct tst_cg_group *const cg,
                const char *const file_name)
__attribute__ ((nonnull, warn_unused_result));

#define SAFE_CG_READ(cg, file_name, out, len)			\
	safe_cg_read(__FILE__, __LINE__,				\
			 (cg), (file_name), (out), (len))

ssize_t safe_cg_read(const char *const file, const int lineno,
                     const struct tst_cg_group *const cg,
                     const char *const file_name,
                     char *const out, const size_t len)
__attribute__ ((nonnull));

#define SAFE_CG_PRINTF(cg, file_name, fmt, ...)			\
	safe_cg_printf(__FILE__, __LINE__,				\
			   (cg), (file_name), (fmt), __VA_ARGS__)

#define SAFE_CG_PRINT(cg, file_name, str)				\
	safe_cg_printf(__FILE__, __LINE__, (cg), (file_name), "%s", (str))

void safe_cg_printf(const char *const file, const int lineno,
                    const struct tst_cg_group *const cg,
                    const char *const file_name,
                    const char *const fmt, ...)
__attribute__ ((format (printf, 5, 6), nonnull));

#define SAFE_CG_OPEN(cg, file_name, flags, fds)			\
	safe_cg_open(__FILE__, __LINE__, (cg), (file_name), (flags), (fds))

int safe_cg_open(const char *const file, const int lineno,
                 const struct tst_cg_group *const cg,
                 const char *const file_name,
                 int flags, int *fds)
__attribute__ ((nonnull));

#define SAFE_CG_FCHOWN(cg, file_name, owner, group)		\
	safe_cg_fchown(__FILE__, __LINE__, (cg), (file_name), (owner), (group))

void safe_cg_fchown(const char *const file, const int lineno,
                    const struct tst_cg_group *const cg,
                    const char *const file_name,
                    uid_t owner, gid_t group)
__attribute__ ((nonnull));

#define SAFE_CG_SCANF(cg, file_name, fmt, ...)			\
	safe_cg_scanf(__FILE__, __LINE__,				\
			  (cg), (file_name), (fmt), __VA_ARGS__)

void safe_cg_scanf(const char *file, const int lineno,
                   const struct tst_cg_group *const cg,
                   const char *const file_name,
                   const char *const fmt, ...)
__attribute__ ((format (scanf, 5, 6), nonnull));

#define SAFE_CG_LINES_SCANF(cg, file_name, fmt, ...)		\
	safe_cg_lines_scanf(__FILE__, __LINE__,			\
				(cg), (file_name), (fmt), __VA_ARGS__)

void safe_cg_lines_scanf(const char *const file, const int lineno,
                         const struct tst_cg_group *const cg,
                         const char *const file_name,
                         const char *const fmt, ...)
__attribute__ ((format (scanf, 5, 6), nonnull));

#define SAFE_CG_OCCURSIN(cg, file_name, needle)		\
	safe_cg_occursin(__FILE__, __LINE__,		\
			     (cg), (file_name), (needle))

int safe_cg_occursin(const char *file, const int lineno,
                     const struct tst_cg_group *const cg,
                     const char *const file_name,
                     const char *const needle);

#endif /* TST_CGROUP_H */

