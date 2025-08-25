This project is a web-based visualizer for Binary Search Trees (BSTs). Its main goal is to allow users to input a list of numbers and see how those numbers are structured into a BST.

How it Works

C++ Backend

The BST logic (insertion, layout, traversal) is implemented in C++.

Duplicates are placed in the left subtree.

After building the tree, each node is assigned an x-position (based on in-order traversal) and a y-position (based on depth).

The tree structure is converted into a JSON format describing nodes and edges.

WebAssembly Compilation

The C++ code is compiled to WebAssembly (WASM) using Emscripten.

This allows the heavy computation and tree logic to run in the browser at near-native speed.

Frontend Visualization

A simple HTML/CSS/JS frontend reads the JSON from the WASM module.

Uses SVG to draw nodes (circles) and edges (lines) dynamically.

Users can interact with the tree by entering numbers and clicking Build or Reset.

Features

Handles positive, negative, and duplicate numbers.

Ties/duplicates are always inserted to the left.

Dynamic SVG layout for clear visualization.

Reset functionality clears the tree and input.

Fully static: can run in any modern browser without a server backend.