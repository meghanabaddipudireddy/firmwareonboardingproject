# Firmware Onboarding Project

## Overview
This project implements simple analog pedal sensing and torque mapping using the mbed framework.  
It read two analog pedal inputs, calculates a normalized pedal percentage and applies different torque profiles (smooth, agressive, and linear) to simulate real-world throttle control.

---

## Torque Profile Mapping
Supports three selectable profiles:

1. Linear: direct mapping between pedal position and torque  
2. Smooth: quadratic response for gentle torque buildup  
3. Agressive: square-root curve for faster torque ramp-up  

*currently set to smooth*

---

## Launch Control Idea

### Slip Signal Detection

**Problem:**  
If applied torque is greater than the available friction, the tires spin faster than the car is actually moving causing tire to "slip".  
In other words, the rear wheel slip is higher than the vehichles forward speed.

**Idea:**  
Compute slip signal from wheel speeds and a reference vehicle speed, filter it, and trigger when it exceeds threshold.

---

### 1. Slip Ratio
We can't directly measure friction but we can measure wheel rotation speed and approximate vehicle forward speed.  
Define a slip ratio:
s = (wheel speed - vehicle speed) / vehicle speed

if s = 0 then the tire is rolling perfectly with full grip  
if s = 0.1 then the wheel is spinning 10% faster than vehicle motion (mild slip)  
if s > 0.2 then there is lots of spin indicating tire slippage  

---

### 2. Reference Vehicle Speed Options
Use either front wheel average or sensor fusion estimator

---

### 3. Filtering and Logic
Filtering is done as raw vehicle speeds are noisy.  
Apply a threashold to be sure the tire is truly slipping and it is not just the noise.

---

### 4. Practical Threshold Starting Points
Trigger slip detection when s > 0.05 to s > 0.15 depending on surface

---

### Alternative Signals
1. Compare rear wheel speed with front wheel speed directly (assumes that front wheels don't slip)  
2. Monitor wheel acceleration  
   If there is a sudden large positive ω at rears while vehicle accel is low, then there is slippage

---

### Prevention

1. **Torque Limit**  
   If rear slip is detected, then clamp the engine torque request to a pre-set percentage.  
   Very simple to implement but could be too agressive or too weak.

2. **Brake-based traction control**  
   Apply small breaking torque to slipping rear wheels until slip reduces.  
   Very effective if engine torque is not available, but could heat breaks, waste energy and affect stability.

3. **Closed-loop slip control (Best Method with our limitations)**  
   First pick a target slip amount. It should be a little more than perfect, so around 10% slip using the slip ratio.  
   If the tires are spinning too much, then back off the power.  
   If the tires aren't spinning enough, then increase the power.  
   Using a controller (PI/PID), the computer is able to tweak the torque to keep the slippage at 10%.

**Control Law:**
error = s - target

If error is positive, then the tires are slipping too much.  
If error is negatie, then they aren't slipping enough.

controller output (PI controller) = Kp * error + Ki * ∫error (makes smooth adjustments)


Kp * error reacts right away to how far off you are (instant correction)  
Ki * ∫error acts as memory correction  

map controller output to a torque reduction on command (clamp it at a max so it doesn't go too far):

- if output is 0 then don't change power  
- if output is 0.3 then cut torque by 30%  
- if output is 0.8 then cut torque by 80%

