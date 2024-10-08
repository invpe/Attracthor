import random
import numpy as np

# Function to generate random dataset with hidden "code"
def generate_dataset_with_code(size, noise_factor=0.7, pattern_length=100, pattern_value=5):
    dataset = []
    
    # Generate a random dataset with values between 0 and 1
    random_data = [random.random() for _ in range(size)]
    
    # Embed a repeating pattern or "code"
    for i in range(0, size, pattern_length * 5):
        if i + pattern_length < size:
            # Insert a pattern at intervals (e.g., a constant value or increasing sequence)
            random_data[i:i + pattern_length] = [pattern_value] * pattern_length
    
    # Add noise to the dataset
    noisy_data = [(val * noise_factor + (random.random() * (1 - noise_factor))) for val in random_data]
    
    return noisy_data

# Save dataset to file
def save_dataset(dataset, filename="secretcode.txt"):
    with open(filename, 'w') as f:
        for value in dataset:
            f.write(f"{value}\n")
    print(f"Dataset saved to {filename}")

# Parameters
dataset_size = 10000
noise_level = 0.75  # Adjust noise level (closer to 1 = less noise, more structure visible)
pattern_len = 1000
pattern_val = 0.75  # Value for the pattern

# Generate and save dataset
dataset = generate_dataset_with_code(dataset_size, noise_factor=noise_level, pattern_length=pattern_len, pattern_value=pattern_val)
save_dataset(dataset)
