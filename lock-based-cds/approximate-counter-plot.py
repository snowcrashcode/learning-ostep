import subprocess
import re
import matplotlib.pyplot as plt

# Function to execute the C program and capture the output
def run_c_program():
    # Execute the C program using subprocess
    process = subprocess.Popen(
        ['./concurrent-locks'],  # Replace with the actual name of your C executable
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    # Capture the output and errors (if any)
    stdout, stderr = process.communicate()

    if stderr:
        print(f"Error encountered: {stderr}")
    
    return stdout

# Function to parse the output from the C program and extract thread counts, elapsed times, and thresholds
def parse_output(output):
    thread_counts = []
    elapsed_times = []
    thresholds = []

    for line in output.splitlines():
        # Look for lines with "Running X threads.." and extract the thread count
        thread_match = re.search(r'Running (\d+) threads', line)
        if thread_match:
            thread_counts.append(int(thread_match.group(1)))

        # Look for lines with "Time elapsed: Y s" and extract the time
        time_match = re.search(r'Time elapsed: ([\d\.]+)', line)
        if time_match:
            elapsed_times.append(float(time_match.group(1)))

        # Look for lines with "Threshold used: X" and extract the threshold
        threshold_match = re.search(r'Threshold used: (\d+)', line)
        if threshold_match:
            thresholds.append(int(threshold_match.group(1)))

    return thread_counts, elapsed_times, thresholds

# Function to plot the results
def plot_results(thread_counts, elapsed_times, thresholds):
    plt.figure(figsize=(10, 6))
    plt.plot(thread_counts, elapsed_times, marker='o')

    plt.title('Time Elapsed vs Number of Threads', fontsize=14)
    plt.xlabel('Number of Threads', fontsize=12)
    plt.ylabel('Time Elapsed (seconds)', fontsize=12)
    plt.grid(True)

    # Display threshold information on the graph
    if len(set(thresholds)) == 1:  # If there's only one threshold, show it in the title
        plt.title(f'Time Elapsed vs Number of Threads (Threshold = {thresholds[0]})', fontsize=14)
    else:  # Otherwise, annotate the points with their thresholds
        for i, txt in enumerate(thresholds):
            plt.annotate(f'{txt}', (thread_counts[i], elapsed_times[i]))

    plt.savefig('approximate_time_elapsed_vs_threads.png')
    print("Saved plot to 'approximate_time_elapsed_vs_threads.png'")


# Main function
def main():
    # Run the C program and capture its output
    output = run_c_program()

    # Parse the output from the C program
    thread_counts, elapsed_times, thresholds = parse_output(output)

    # Plot the results
    plot_results(thread_counts, elapsed_times, thresholds)

if __name__ == '__main__':
    main()
