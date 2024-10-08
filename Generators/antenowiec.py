# A0 Data gathering script for https://github.com/invpe/Attracthor
import requests
import time

def get_sensor_data(url):
    try:
        response = requests.get(url)
        if response.status_code == 200:
            data = response.json()
            return data.get("value")
        else:
            print(f"Failed to fetch data. Status code: {response.status_code}")
            return None
    except requests.exceptions.RequestException as e:
        print(f"Error while requesting data: {e}")
        return None

def save_data_to_file(value, file_name):
    with open(file_name, 'a') as file:  # Open file in append mode
        file.write(f"{value}\n")  # Write the value on a new line

def main():
    url = "http://192.168.1.230/get"
    file_counter = 1
    sample_counter = 0
    sample_limit = 10000
    file_name = f"antenowiec_{file_counter}.txt"
    
    while True:
        # Get the sensor data
        value = get_sensor_data(url)
        
        if value is not None:
            # Save the value to the current file
            save_data_to_file(value, file_name)
            sample_counter += 1
        
            # Print debug information
            print(f"File: {file_name} | Gathered: {sample_counter} | Remaining: {sample_limit - sample_counter}")
        
        # Check if we've reached the sample limit
        if sample_counter >= sample_limit:
            print(f"Sample limit reached for {file_name}. Starting new file.")
            # Start a new file
            file_counter += 1
            file_name = f"antenowiec_{file_counter}.txt"
            sample_counter = 0  # Reset sample counter
        
        # Small delay between requests (adjust as needed)
        time.sleep(1)

if __name__ == "__main__":
    main()
