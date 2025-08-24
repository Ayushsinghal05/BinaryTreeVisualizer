#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

struct Node {
    int id;
    int val;
    Node* left;
    Node* right;
    Node* parent;
    int depth;
    int xindex; // in-order index
    Node(int _id, int _val): id(_id), val(_val), left(nullptr), right(nullptr), parent(nullptr), depth(0), xindex(0) {}
};

static Node* insert(Node* root, Node* parent, int val, int &idcounter){
    if(!root){
        Node* n = new Node(++idcounter, val);
        n->parent = parent;
        n->depth = parent ? parent->depth + 1 : 0;
        return n;
    }
    if(val <= root->val){
        root->left = insert(root->left, root, val, idcounter);
    } else {
        // duplicates go to the right
        root->right = insert(root->right, root, val, idcounter);
    }
    return root;
}

static void parse_ints(const std::string &s, std::vector<int> &out){
    // Accepts separators: space, comma, newline; supports negative numbers.
    std::stringstream cur;
    bool inNum=false;
    for(char ch: s){
        if(std::isdigit(ch) || ch=='-' || ch=='+'){
            cur << ch; inNum=true;
        } else {
            if(inNum){
                int v; cur >> v; out.push_back(v);
                cur.clear(); cur.str(""); inNum=false;
            }
        }
    }
    if(inNum){
        int v; cur >> v; out.push_back(v);
    }
}

static void assign_inorder_x(Node* root){
    int idx=0;
    std::function<void(Node*)> dfs = [&](Node* n){
        if(!n) return;
        dfs(n->left);
        n->xindex = idx++;
        dfs(n->right);
    };
    dfs(root);
}

static void collect_nodes(Node* root, std::vector<Node*>& nodes){
    if(!root) return;
    collect_nodes(root->left, nodes);
    nodes.push_back(root);
    collect_nodes(root->right, nodes);
}

static void delete_tree(Node* n){
    if(!n) return;
    delete_tree(n->left);
    delete_tree(n->right);
    delete n;
}

// Build the BST from an input string, compute a simple tidy-ish layout, and return a JSON string.
std::string build_tree_json(const std::string &input){
    std::vector<int> vals; vals.reserve(256);
    parse_ints(input, vals);
    if(vals.empty()) return std::string("{\"nodes\":[],\"edges\":[]}");

    Node* root=nullptr; int idc=0;
    for(int v: vals){
        root = insert(root, nullptr, v, idc);
    }

    // In-order x positions, y by depth
    assign_inorder_x(root);

    // Collect nodes in in-order for stable output
    std::vector<Node*> nodes; nodes.reserve(idc);
    collect_nodes(root, nodes);

    // Build JSON (manual to avoid dependencies)
    // nodes: {id, value, xindex, depth, parent}
    // edges: {from, to}
    std::ostringstream json;
    json << "{\"nodes\":[";
    bool first=true;
    for(Node* n: nodes){
        if(!first) json << ","; first=false;
        json << "{\"id\":" << n->id
             << ",\"value\":" << n->val
             << ",\"x\":" << n->xindex
             << ",\"y\":" << n->depth
             << ",\"parent\":" << (n->parent ? n->parent->id : 0)
             << "}";
    }
    json << "],\"edges\":[";
    first=true;
    // BFS edges
    std::function<void(Node*)> addEdges = [&](Node* n){
        if(!n) return;
        if(n->left){ if(!first) json << ","; first=false; json << "{\"from\":"<<n->id<<",\"to\":"<<n->left->id<<"}"; }
        if(n->right){ if(!first) json << ","; first=false; json << "{\"from\":"<<n->id<<",\"to\":"<<n->right->id<<"}"; }
        addEdges(n->left);
        addEdges(n->right);
    };
    addEdges(root);
    json << "]}";

    std::string out = json.str();

    // clean up heap
    delete_tree(root);
    return out;
}

EMSCRIPTEN_BINDINGS(bst_module) {
    emscripten::function("build_tree_json", &build_tree_json);
}