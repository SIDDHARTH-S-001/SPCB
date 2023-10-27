import csv
import matplotlib.pyplot as plt

# Function to read points from a CSV file
def read_points_from_csv(csv_filename):
    points = []
    with open(csv_filename, 'r') as csvfile:
        csvreader = csv.reader(csvfile)
        next(csvreader)  # Skip the header row
        for row in csvreader:
            x, y = map(float, row)
            points.append((x, y))
    return points

# Read points from the CSV files
blue_line_points = read_points_from_csv('blue_line_points_scaled.csv')
pink_line_points = read_points_from_csv('pink_line_points_scaled.csv')

# Create a plot
plt.figure(figsize=(8, 6))
plt.title("Graph Points")
plt.xlabel("X (Units)")
plt.ylabel("Y (Units")

# Plot the blue and pink points
blue_x, blue_y = zip(*blue_line_points)
pink_x, pink_y = zip(*pink_line_points)

plt.plot(blue_x, blue_y, 'b.', label='Blue Line Points')
plt.plot(pink_x, pink_y, 'r.', label='Pink Line Points')

# Add axes
plt.axhline(0, color='black', linewidth=0.5)
plt.axvline(0, color='black', linewidth=0.5)

# # Annotate each point with its coordinates
# for x, y in blue_line_points:
#     plt.annotate(f'({x:.2f}, {y:.2f})', (x, y), textcoords="offset points", xytext=(0, 10), ha='center', fontsize=8, color='blue')

# for x, y in pink_line_points:
#     plt.annotate(f'({x:.2f}, {y:.2f})', (x, y), textcoords="offset points", xytext=(0, 10), ha='center', fontsize=8, color='red')

# Show legend
plt.legend()

# Show the plot
plt.grid()
plt.show()
