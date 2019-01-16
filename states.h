#ifndef _states_h
#define _states_h

/** \defgroup States States
 * @{
 */

#define STATE_START     0   /**< Power has just been applied to mc */
#define STATE_POWERON   1   /**< Assert EN */
#define STATE_BOOTWAIT0 2   /**< Set bootwait timer */
#define STATE_BOOTWAIT1 3   /**< Wait for Pi to assert BOOT or timer expiry */
#define STATE_BOOT      4   /**< System has booted */
#define STATE_SHUTDOWN0 5   /**< Set shutdown timer */
#define STATE_SHUTDOWN1 6   /**< Wait for Pi to de-assert BOOT or timer expiry */
#define STATE_POWEROFF0 7   /**< Set poweroff timer */
#define STATE_POWEROFF1 8   /**< Wait for timer expiry */
#define STATE_POWEROFF2 9   /**< Power off */
#define STATE_IDLE      10  /**< Wait for power button press */

/** @} */

#endif // _states_h
