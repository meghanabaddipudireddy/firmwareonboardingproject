# Firmware Onboarding Project

## Overview
This project implements simple analog pedal sensing and torque mapping using the **mbed** framework.  
It reads two analog pedal inputs, calculates a normalized pedal percentage, and applies different torque profiles (smooth, aggressive, and linear) to simulate real-world throttle control.

---

## Torque Profile Mapping
Supports three selectable profiles:

1. **Linear:** direct mapping between pedal position and torque  
2. **Smooth:** quadratic response for gentle torque buildup  
3. **Aggressive:** square-root curve for faster torque ramp-up  

*Currently set to smooth.*

---

## Launch Control Idea

### Slip Signal Detection

**Problem:**  
If applied torque is greater than the available friction, the tires spin faster than the car is actually moving, causing tire “slip.”  
In other words, the rear wheel slip is higher than the vehicle’s forward speed.

**Idea:**  
Compute a slip signal from wheel speeds and a reference vehicle speed, filter it, and trigger when it exceeds a threshold.

---

### 1. Slip Ratio
We can't directly measure friction, but we can measure wheel rotation speed and approximate vehicle forward speed.  
Define a slip ratio:

- If `s = 0`, then the tire is rolling perfectly with full grip  
- If `s = 0.1`, then the wheel is spinning 10% faster than vehicle motion (mild slip)  
- If `s > 0.2`, then there is lots of spin indicating tire slippage  

---

### 2. Reference Vehicle Speed Options
- Use either front wheel average, or  
- Sensor fusion estimator  

---

### 3. Filtering and Logic
Filtering is done because raw vehicle speeds are noisy.  
Apply a threshold to be sure the tire is truly slipping and it is not just the noise.

---

### 4. Practical Threshold Starting Points
Trigger slip detection when:

- `s > 0.05` → very sensitive surfaces  
- `s > 0.15` → normal driving conditions  

---

### Alternative Signals
1. **Compare rear wheel speed with front wheel speed directly** (assumes that front wheels don’t slip)  
2. **Monitor wheel acceleration:**  
   If there is a sudden large positive ω at rears while vehicle accel is low, then there is slippage.

---

## Prevention

### 1. Torque Limit
If rear slip is detected, then clamp the engine torque request to a pre-set percentage.  
Very simple to implement but could be too aggressive or too weak.

---

### 2. Brake-Based Traction Control
Apply small braking torque to slipping rear wheels until slip reduces.  
Very effective if engine torque is not available, but could heat brakes, waste energy, and affect stability.

---

### 3. Closed-Loop Slip Control (Best Method with Our Limitations)
First pick a target slip amount.  
It should be a little more than perfect — around 10% slip using the slip ratio.  

If the tires are spinning too much, then back off the power.  
If the tires aren't spinning enough, then increase the power.  

Using a controller (PI/PID), the computer can tweak the torque to keep the slippage at 10%.

**Control Law:**


error = s - target

- If **error is positive**, then the tires are slipping too much.  
- If **error is negative**, then they aren't slipping enough.


controller output(PI controller) = Kp * error + Ki * ∫error (makes smooth adjustments)

- `Kp * error` reacts right away to how far off you are (instant correction)  
- `Ki * ∫error` acts as memory correction  

Map controller output to a torque reduction command (clamp it at a max so it doesn't go too far):

| Controller Output | Effect |
|--------------------|--------|
| 0.0 | Don’t change power |
| 0.3 | Cut torque by 30% |
| 0.8 | Cut torque by 80% |

---

This approach ensures the vehicle maintains optimal traction without overcutting torque or allowing excessive wheel spin.





