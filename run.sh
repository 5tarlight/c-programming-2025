# Check if the build directory exists
if [ ! -d "build" ]; then
  mkdir build
else
  rm -rf build
  mkdir build
fi

# Navigate to the build directory
cd build || { echo "Failed to navigate to the build directory"; exit 1; }

# Run cmake to configure the project
cmake .. || { echo "CMake configuration failed"; exit 1; }

# Build the project
make || { echo "Build failed"; exit 1; }

# Run the program
read -n 1 -s -r -p "Press any key to continue..."
./c_programming || { echo "Program execution failed"; exit 1; }

# Clean up the build directory
cd .. || { echo "Failed to navigate back to the root directory"; exit 1; }
rm -rf build || { echo "Failed to remove the build directory"; exit 1; }
