# import cv2
# import numpy as np

# # Load the image
# image = cv2.imread(r"C:\Users\Tamil Vendan\Downloads\20200126201610_47576.png")

# # Convert the image to HSV color space
# hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

# # Define the lower and upper bounds of the color in HSV format
# lower_color = np.array([110, 50, 50])  # Adjust these values as needed
# upper_color = np.array([130, 255, 255])  # Adjust these values as needed

# # Create a mask to identify the pixels with the specified color
# mask = cv2.inRange(hsv_image, lower_color, upper_color)

# # Find the contours in the mask
# contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# # Extract the coordinates of the line (contour)
# if len(contours) > 0:
#     contour = contours[0]  # You can change this to access other contours if needed
#     for point in contour:
#         x, y = point[0]
#         print(f"Pixel X: {x}, Pixel Y: {y}")

# # Display the mask (for visualization purposes)
# cv2.imshow('Color Mask', mask)
# cv2.waitKey(0)
# cv2.destroyAllWindows()


# import cv2
# import numpy as np
# import csv

# def extract_line_points(image, lower_color, upper_color):
#     # Convert the image to HSV color space
#     hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

#     # Create a mask to identify the pixels with the specified color
#     mask = cv2.inRange(hsv_image, lower_color, upper_color)

#     # Find the contours in the mask
#     contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

#     # Extract the coordinates of the line (contour)
#     line_points = []
#     for contour in contours:
#         for point in contour:
#             x, y = point[0]
#             line_points.append((x, y))
    
#     return line_points

# # Load the image
# image = cv2.imread(r"C:\Users\Tamil Vendan\Downloads\20200126201610_47576.png")

# # Define the lower and upper bounds of the blue and pink colors in HSV format
# blue_lower_color = np.array([110, 50, 50])  # Adjust these values as needed
# blue_upper_color = np.array([130, 255, 255])  # Adjust these values as needed

# pink_lower_color = np.array([150, 50, 50])  # Adjust these values as needed
# pink_upper_color = np.array([170, 255, 255])  # Adjust these values as needed

# # Extract points for the blue and pink lines
# blue_line_points = extract_line_points(image, blue_lower_color, blue_upper_color)
# pink_line_points = extract_line_points(image, pink_lower_color, pink_upper_color)

# # Save the points to separate CSV files
# with open('blue_line_points.csv', 'w', newline='') as blue_csvfile:
#     csvwriter = csv.writer(blue_csvfile)
#     csvwriter.writerow(['x', 'y'])
#     for x, y in blue_line_points:
#         csvwriter.writerow([x, y])

# with open('pink_line_points.csv', 'w', newline='') as pink_csvfile:
#     csvwriter = csv.writer(pink_csvfile)
#     csvwriter.writerow(['x', 'y'])
#     for x, y in pink_line_points:
#         csvwriter.writerow([x, y])

# print("Points saved to 'blue_line_points.csv' and 'pink_line_points.csv'.")

import cv2
import numpy as np
import csv

def extract_line_points(image, lower_color, upper_color, y_scale_factor, x_scale_factor):
    # Convert the image to HSV color space
    hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # Create a mask to identify the pixels with the specified color
    mask = cv2.inRange(hsv_image, lower_color, upper_color)

    # Find the contours in the mask
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Extract the coordinates of the line (contour) and apply scale transformation
    line_points = []
    for contour in contours:
        for point in contour:
            x, y = point[0]

            # Apply the scale transformation
            scaled_x = x * x_scale_factor
            scaled_y = 2.4 + (y * y_scale_factor)  # Invert y-axis and apply scaling

            line_points.append((scaled_x, scaled_y))
    
    return line_points

# Load the image
image = cv2.imread(r"C:\Users\Tamil Vendan\Downloads\20200126201610_47576.png")

# Define the lower and upper bounds of the blue and pink colors in HSV format
blue_lower_color = np.array([110, 50, 50])  # Adjust these values as needed
blue_upper_color = np.array([130, 255, 255])  # Adjust these values as needed

pink_lower_color = np.array([150, 50, 50])  # Adjust these values as needed
pink_upper_color = np.array([170, 255, 255])  # Adjust these values as needed

# Define the scale factors
x_scale_factor = 0.1 / 17  # 0.1 for every 17 pixels in the y-axis
y_scale_factor = 0.6 / 20  # 0.6 for every 20 pixels in the x-axis

# Extract points for the blue and pink lines with scale transformation
blue_line_points = extract_line_points(image, blue_lower_color, blue_upper_color, y_scale_factor, x_scale_factor)
pink_line_points = extract_line_points(image, pink_lower_color, pink_upper_color, y_scale_factor, x_scale_factor)

# Save the scaled points to separate CSV files
with open('blue_line_points_scaled.csv', 'w', newline='') as blue_csvfile:
    csvwriter = csv.writer(blue_csvfile)
    csvwriter.writerow(['x', 'y'])
    for x, y in blue_line_points:
        csvwriter.writerow([x, y])

with open('pink_line_points_scaled.csv', 'w', newline='') as pink_csvfile:
    csvwriter = csv.writer(pink_csvfile)
    csvwriter.writerow(['x', 'y'])
    for x, y in pink_line_points:
        csvwriter.writerow([x, y])

print("Scaled points saved to 'blue_line_points_scaled.csv' and 'pink_line_points_scaled.csv'.")
