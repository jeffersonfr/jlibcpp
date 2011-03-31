#include <stdint.h>

#define MAX_RGB 				255
#define MAX_TREE_DEPTH 	8
#define MAX_NODES 			500000

enum jquantization_mode_t {
	QUICK_MATCH,
	CLOSE_MATCH
};

/**
 * \brief The result of a closest color search.
 *
 */
struct jmean_search_t {
	int distance;
	int color_number;
};

class Cube;

/**
 * \brief A single Node in the tree.
 *
 */
class Node {

	friend class Cube;

	private:
		Node *_parent;
		Cube *_cube;
		Node **_child;
		int _nchild;
		int _id;
		int _level;
		int _mid_red;
		int _mid_green;
		int _mid_blue;
		int _number_pixels;
		int _unique;
		int _total_red;
		int _total_green;
		int _total_blue;
		int _color_number;

	public:
		/**
		 * \brief
		 *
		 */
		Node(Cube *cube);

		/**
		 * \brief
		 *
		 */
		Node(Node *parent, int id, int level);

		/**
		 * \brief
		 *
		 */
		virtual ~Node();

		/**
		 * \brief Figure out the distance between this node and som color.
		 *
		 */
		static int distance(uint32_t color, int red, int green, int blue);

		/**
		 * \brief Remove this child node, and make sure our parent absorbs our pixel statistics.
		 *
		 */
		void pruneChild();

		/**
		 * \brief Prune the lowest layer of the tree.
		 *
		 */
		void pruneLevel();

		/**
		 * \brief Remove any nodes that have fewer than threshold pixels. Also, as long as we're walking the tree:
		 *
		 *  - figure out the color with the fewest pixels
		 *  - recalculate the total number of colors in the tree
		 */
		int reduce(int threshold, int next_threshold);

		/*
		 * \brief colormap traverses the color cube tree and notes each colormap entry. A colormap entry is any node in the
		 * color cube tree where the number of unique colors is not zero.
		 */
		void colormap();

		/** 
		 * \brief ClosestColor traverses the color cube tree at a particular node and determines which colormap entry best represents the input color.
		 *
		 */
		void closestColor(int red, int green, int blue, jmean_search_t *search);

};

class Cube {

	friend class Node;

	private:
		Node *_root;
		uint32_t *_pixels;
		uint32_t *_colormap;
		int _pixels_size;
		int _max_colors;
		int _depth;
		int _colors;
		int _nodes;
		jquantization_mode_t _mode;

	public:
		/**
		 * \brief
		 *
		 */
		Cube(uint32_t *pixels, int pixels_size, int max_colors);

		/**
		 * \brief
		 *
		 */
		virtual ~Cube();

		/**
		 * \brief
		 *
		 */
		void SetMode(jquantization_mode_t mode);

		/**
		 * \brief
		 *
		 */
		jquantization_mode_t GetMode();

		/*
		 * \brief Procedure Classification begins by initializing a color  description tree of sufficient depth to represent each possible input color in a leaf. However, it is impractical
		 * to generate a fully-formed color description tree in the classification phase for realistic values of cmax. If colors components in the input image are quantized to k-bit
		 * precision, so that cmax= 2k-1, the tree would need k levels below the root node to allow representing each possible input color in a leaf. This becomes prohibitive because the
		 * tree's total number of nodes is 1 + sum(i=1,k,8k).
		 *
		 * A complete tree would require 19,173,961 nodes for k = 8, cmax = 255. Therefore, to avoid building a fully populated tree, QUANTIZE: (1) Initializes data structures for nodes only 
		 * as they are needed; (2) Chooses a maximum depth for the tree as a function of the desired number of colors in the output image (currently log2(colormap size)).
		 *
		 * For each pixel in the input image, classification scans downward from the root of the color description tree. At each level of the tree it identifies the single node which represents 
		 * a cube in RGB space containing It updates the following data for each such node:
		 *
		 * number_pixels : Number of pixels whose color is contained in the RGB cube which this node represents;
		 * unique : Number of pixels whose color is not represented in a node at lower depth in the tree; initially, n2 = 0 for all nodes except leaves of the tree.
		 * total_red/green/blue : Sums of the red, green, and blue component values for all pixels not classified at a lower depth. The combination of these sums and n2 will ultimately 
		 * characterize the mean color of a set of pixels represented by this node.
		 */
		void classification();

		/*
		 * \brief reduction repeatedly prunes the tree until the number of nodes with unique > 0 is less than or equal to the maximum number of colors allowed in the output image.
		 *
		 * When a node to be pruned has offspring, the pruning procedure invokes itself recursively in order to prune the tree from the leaves upward.  The statistics of the node
		 * being pruned are always added to the corresponding data in that node's parent.  This retains the pruned node's color characteristics for later averaging.
		 */
		void reduction();

		/*
		 * \brief Procedure assignment generates the output image from the pruned tree. The output image consists of two parts: (1) A color map, which is an array of color descriptions (RGB
		 * triples) for each color present in the output image; (2) A pixel array, which represents each pixel as an index into the color map array.
		 *
		 * First, the assignment phase makes one pass over the pruned color description tree to establish the image's color map. For each node with n2 > 0, it divides Sr, Sg, and Sb by n2.
		 * This produces the mean color of all pixels that classify no lower than this node. Each of these colors becomes an entry in the color map.
		 *
		 * Finally, the assignment phase reclassifies each pixel in the pruned tree to identify the deepest node containing the pixel's color. The pixel's value in the pixel array becomes
		 * the index of this node's mean color in the color map.
		 */
		void assignment();

		/**
		 * \brief
		 *
		 */
		void GetPalette(uint32_t **pixels);

};

class Quantization {

	private:
		static Quantization *_instance;

	private:
		/**
		 * \brief
		 *
		 */
		Quantization();

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~Quantization();

		/**
		 * \brief
		 *
		 */
		static Quantization * GetInstance();

		/**
		 * \brief Reduce the image to the given number of colors. The pixels are reduced in place.
		 *
		 * \return The new color palette.
		 */
		static Cube * quantizeImage(uint32_t *pixels, int pixels_size, int max_colors);

};

