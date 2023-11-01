import numpy as np

# Parameters for generating a 1D chaotic dataset
iterations = 10000
x = np.empty(iterations)
x[0] = 0.11  # Initial value

# Logistic map function
def logistic_map(x):
    r = 4.0  # Adjust this parameter for different chaotic behavior
    return r * x * (1 - x)

# Generate the dataset
for i in range(1, iterations):
    x[i] = logistic_map(x[i - 1])

# Save the dataset to a text file
with open('input.txt', 'w') as file:
    for value in x:
        file.write(f"{value}\n")
