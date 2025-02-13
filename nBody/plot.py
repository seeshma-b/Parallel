#chatGPT generated
import math
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import sys

arrow_scale = 1.

def parse_nbody_output(file_path):
    """
    Parse the n-body simulation output file.

    Parameters:
        file_path (str): Path to the output file.

    Returns:
        list: A list of time steps, where each time step contains a list of particle data.
              Each particle data is a dictionary with keys: 'mass', 'x', 'y', 'z', 'vx', 'vy', 'vz', 'fx', 'fy', 'fz'.
    """
    time_steps = []

    with open(file_path, 'r') as file:
        for line in file:
            data = line.split('\t')
            nbpart = int(data[0])
            particles = []

            index = 1
            for _ in range(nbpart):
                particle = {
                    'mass': float(data[index]),
                    'x': float(data[index + 1]),
                    'y': float(data[index + 2]),
                    'z': float(data[index + 3]),
                    'vx': float(data[index + 4]),
                    'vy': float(data[index + 5]),
                    'vz': float(data[index + 6]),
                    'fx': float(data[index + 7]),
                    'fy': float(data[index + 8]),
                    'fz': float(data[index + 9]),
                }
                particles.append(particle)
                index += 10

            time_steps.append(particles)

    return time_steps

def plot_nbody_trajectories(time_steps, output_pdf):
    """
    Plot the (x, y) positions of particles for each time step and save to a PDF.

    Parameters:
        time_steps (list): List of time steps with particle data.
        output_pdf (str): Path to the output PDF file.
    """
    all_x = [p['x'] for step in time_steps for p in step]
    all_y = [p['y'] for step in time_steps for p in step]
    x_min, x_max = min(all_x), max(all_x)
    y_min, y_max = min(all_y), max(all_y)
    
    with PdfPages(output_pdf) as pdf:
        for t, particles in enumerate(time_steps):
            plt.figure(figsize=(8, 8))
            plt.title(f"Time Step {t + 1}")
            plt.xlabel("x-coordinate")
            plt.ylabel("y-coordinate")

            x_coords = [p['x'] for p in particles]
            y_coords = [p['y'] for p in particles]
            vx = [p['vx'] for p in particles]
            vy = [p['vy'] for p in particles]
            
            plt.scatter(x_coords, y_coords, s=10, c='blue', label="Particles")
            # Add velocity arrows
            for x, y, vx_i, vy_i in zip(x_coords, y_coords, vx, vy):
                plt.arrow(x, y, vx_i*arrow_scale, vy_i*arrow_scale, length_includes_head=True, fc='red', ec='red')
                # width=(math.sqrt(vx_i*vx_i+vy_i*vy_i))

            plt.xlim(x_min, x_max)
            plt.ylim(y_min, y_max)

                
            plt.legend()
            plt.grid(True)

            pdf.savefig()  # Save the current figure to the PDF
            plt.close()

if __name__ == "__main__":
    input_file = sys.argv[1]  # input of the plotter-- aka output of the sim code
    output_file = sys.argv[2]  # need to be .pdf
    if len(sys.argv) == 4:
        arrow_scale = float(sys.argv[3])
    
    time_steps = parse_nbody_output(input_file)
    plot_nbody_trajectories(time_steps, output_file)

    print(f"Plots saved to {output_file}")
