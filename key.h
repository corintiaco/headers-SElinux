* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
        * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
        * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
        * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
        * OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#ifndef KEY_H
#define KEY_H

#include "rdbx.h" /* for srtp_xtd_seq_num_t */
#include "err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct srtp_key_limit_ctx_t *srtp_key_limit_t;

typedef enum {
    srtp_key_event_normal,
    srtp_key_event_soft_limit,
    srtp_key_event_hard_limit
} srtp_key_event_t;

srtp_err_status_t srtp_key_limit_set(srtp_key_limit_t key,
                                     const srtp_xtd_seq_num_t s);

srtp_err_status_t srtp_key_limit_clone(srtp_key_limit_t original,
                                       srtp_key_limit_t *new_key);

srtp_err_status_t srtp_key_limit_check(const srtp_key_limit_t key);

srtp_key_event_t srtp_key_limit_update(srtp_key_limit_t key);

typedef enum {
    srtp_key_state_normal,
    srtp_key_state_past_soft_limit,
    srtp_key_state_expired
} srtp_key_state_t;

typedef struct srtp_key_limit_ctx_t {
    srtp_xtd_seq_num_t num_left;
    srtp_key_state_t state;
} srtp_key_limit_ctx_t;

#ifdef __cplusplus
}
#endif

#endif /* KEY_H */
