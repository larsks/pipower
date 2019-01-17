#ifndef _states_h
#define _states_h

/** \defgroup States States
 * @{
 */

#define STATE_START         0   /**< Power has just been applied to mc */
#define STATE_POWERWAIT0    1   /**< Set powerwait timer */
#define STATE_POWERWAIT1    2   /**< Wait for USB signal to stabilize */
#define STATE_POWERON       3   /**< Assert EN */
#define STATE_BOOTWAIT0     4   /**< Set bootwait timer */
#define STATE_BOOTWAIT1     5   /**< Wait for Pi to assert BOOT or timer expiry */
#define STATE_BOOT          6   /**< System has booted */
#define STATE_SHUTDOWN0     7   /**< Set shutdown timer */
#define STATE_SHUTDOWN1     8   /**< Wait for Pi to de-assert BOOT or timer expiry */
#define STATE_POWEROFF0     9   /**< Set poweroff timer */
#define STATE_POWEROFF1     10  /**< Wait for timer expiry */
#define STATE_POWEROFF2     11  /**< Power off */
#define STATE_IDLE0         12  /**< Enter low-power mode */
#define STATE_IDLE1         13  /**< Start idle timer */
#define STATE_IDLE2         14  /**< Wait for power button press */

/** @} */

#endif // _states_h
