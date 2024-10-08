import numpy as np
from scipy.integrate import solve_ivp

# Lorenz system parameters
sigma = 10.0
beta = 8.0 / 3.0
rho = 28.0

# Lorenz system equations
def lorenz_system(t, state):
    x, y, z = state
    dxdt = sigma * (y - x)
    dydt = x * (rho - z) - y
    dzdt = x * y - beta * z
    return [dxdt, dydt, dzdt]

# Initial state (near the origin, but not exactly at the fixed point)
initial_state = [1.0, 1.0, 1.0]

# Time range to solve the differential equations over
t_span = (0.0, 5000.0)  # from t=0 to t=50
t_eval = np.linspace(*t_span, 100000)  # generate 100000 points in time

# Solve the differential equations
sol = solve_ivp(lorenz_system, t_span, initial_state, t_eval=t_eval)

# Extract the solution for each variable
x, y, z = sol.y

# Define a scaling factor
scaling_factor = 100.0  # or any other factor you choose

# Scale the data
z_scaled = z * scaling_factor

# Save the scaled data to a text file
np.savetxt('lorenz.txt', z_scaled)
