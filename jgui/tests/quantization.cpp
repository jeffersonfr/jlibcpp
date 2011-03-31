#include "quantization.h"

#include <stdlib.h>
#include <values.h>
#include <stdio.h>

static uint32_t *SQUARES = NULL;
static uint32_t *SHIFT = NULL;

Node::Node(Cube *cube) 
{
	_nchild = 0;
	_unique = 0;
	_total_red = 0;
	_total_green = 0;
	_total_blue = 0;
	_color_number = 0;
	_cube = cube;
	_parent = this;
	_id = 0;
	_level = 0;
	_number_pixels = INT_MAX;

	_child = new Node*[8];

	for (int i=0; i<8; i++) {
		_child[i] = NULL;
	}

	_mid_red   = (MAX_RGB + 1) >> 1;
	_mid_green = (MAX_RGB + 1) >> 1;
	_mid_blue  = (MAX_RGB + 1) >> 1;
}

Node::Node(Node *parent, int id, int level) 
{
	_nchild = 0;
	_number_pixels = 0;
	_unique = 0;
	_total_red = 0;
	_total_green = 0;
	_total_blue = 0;
	_color_number = 0;
	_cube = parent->_cube;
	_parent = parent;
	_id = id;
	_level = level;

	_child = new Node*[8];

	for (int i=0; i<8; i++) {
		_child[i] = NULL;
	}

	// add to the cube
	++_cube->_nodes;

	if (level == _cube->_depth) {
		++_cube->_colors;
	}

	// add to the parent
	++parent->_nchild;
	parent->_child[id] = this;

	// figure out our midpoint
	int bi = (1 << (MAX_TREE_DEPTH - level)) >> 1;

	_mid_red   = parent->_mid_red   + ((id & 1) > 0 ? bi : -bi);
	_mid_green = parent->_mid_green + ((id & 2) > 0 ? bi : -bi);
	_mid_blue  = parent->_mid_blue  + ((id & 4) > 0 ? bi : -bi);
}

Node::~Node() 
{
}

int Node::distance(uint32_t color, int r, int g, int b) 
{
	return (SQUARES[((color >> 16) & 0xff) - r + MAX_RGB] + SQUARES[((color >>  8) & 0xff) - g + MAX_RGB] + SQUARES[((color >>  0) & 0xff) - b + MAX_RGB]);
}

void Node::pruneChild() 
{
	--_parent->_nchild;
	--_cube->_nodes;

	_parent->_unique += _unique;
	_parent->_total_red += _total_red;
	_parent->_total_green += _total_green;
	_parent->_total_blue += _total_blue;
	_parent->_child[_id] = NULL;

	_cube = NULL;
	_parent = NULL;
}

void Node::pruneLevel() 
{
	if (_nchild != 0) {
		for (int id=0; id<8; id++) {
			if (_child[id] != NULL) {
				_child[id]->pruneLevel();
			}
		}
	}

	if (_level == _cube->_depth) {
		pruneChild();
	}
}

int Node::reduce(int threshold, int next_threshold) 
{
	if (_nchild != 0) {
		for (int id = 0; id < 8; id++) {
			if (_child[id] != NULL) {
				next_threshold = _child[id]->reduce(threshold, next_threshold);
			}
		}
	}
	
	if (_number_pixels <= threshold) {
		pruneChild();
	} else {
		if (_unique != 0) {
			_cube->_colors++;
		}
		if (_number_pixels < next_threshold) {
			next_threshold = _number_pixels;
		}
	}

	return next_threshold;
}

void Node::colormap() 
{
	if (_nchild != 0) {
		for (int id = 0; id < 8; id++) {
			if (_child[id] != NULL) {
				_child[id]->colormap();
			}
		}
	}
	if (_unique != 0) {
		int r = ((_total_red   + (_unique >> 1)) / _unique);
		int g = ((_total_green + (_unique >> 1)) / _unique);
		int b = ((_total_blue  + (_unique >> 1)) / _unique);

		_cube->_colormap[_cube->_colors] = (0xff << 24 | ((r & 0xff) << 16) | ((g & 0xff) <<  8) | ((b & 0xff) <<  0));

		_color_number = _cube->_colors++;
	}
}

void Node::closestColor(int red, int green, int blue, jmean_search_t *search) 
{
	if (_nchild != 0) {
		for (int id = 0; id < 8; id++) {
			if (_child[id] != NULL) {
				_child[id]->closestColor(red, green, blue, search);
			}
		}
	}

	if (_unique != 0) {
		int color = _cube->_colormap[_color_number];
		int d = distance(color, red, green, blue);

		if (d < search->distance) {
			search->distance = d;
			search->color_number = _color_number;
		}
	}
}

Cube::Cube(uint32_t *pixels, int pixels_size, int max_colors) 
{
	_colormap = NULL;
	_nodes = 0;;
	_mode = CLOSE_MATCH;
	// _mode = QUICK_MATCH;
	_pixels = pixels;
	_pixels_size = pixels_size;
	_colors = 0;
	_max_colors = max_colors;

	int i = max_colors;

	for (_depth=1; i!=0; _depth++) {
		i /= 4;
	}

	if (_depth > 1) {
		--_depth;
	}

	if (_depth > MAX_TREE_DEPTH) {
		_depth = MAX_TREE_DEPTH;
	} else if (_depth < 2) {
		_depth = 2;
	}

	_root = new Node(this);
}

Cube::~Cube()
{
}

void Cube::classification() 
{
	uint32_t *pixels = _pixels;

	for (int i=0; i<_pixels_size; i++) {
		int pixel = pixels[i];
		int red   = (pixel >> 16) & 0xff;
		int green = (pixel >>  8) & 0xff;
		int blue  = (pixel >>  0) & 0xff;

		// a hard limit on the number of nodes in the tree
		if (_nodes > MAX_NODES) {
			_root->pruneLevel();
			--_depth;
		}

		// walk the tree to depth, increasing the number_pixels count for each node
		Node *node = _root;

		for (int level=1; level<=_depth; ++level) {
			int id = (((red > node->_mid_red ? 1 : 0) << 0) | ((green > node->_mid_green ? 1 : 0) << 1) | ((blue  > node->_mid_blue  ? 1 : 0) << 2));

			if (node->_child[id] == NULL) {
				new Node(node, id, level);
			}

			node = node->_child[id];
			node->_number_pixels += SHIFT[level];
		}

		++node->_unique;
		node->_total_red   += red;
		node->_total_green += green;
		node->_total_blue  += blue;
	}
}

void Cube::reduction() 
{
	int threshold = 1;

	while (_colors > _max_colors) {
		_colors = 0;
		threshold = _root->reduce(threshold, INT_MAX);
	}
}

void Cube::assignment() 
{
	uint32_t *pixels = _pixels;

	_colormap = new uint32_t[_colors];

	_colors = 0;
	
	_root->colormap();

	jmean_search_t *search = new jmean_search_t;

	for (int i=0; i<_pixels_size; i++) {
		int pixel = pixels[i];
		int red   = (pixel >> 16) & 0xff;
		int green = (pixel >>  8) & 0xff;
		int blue  = (pixel >>  0) & 0xff;

		// walk the tree to find the cube containing that color
		Node *node = _root;

		for ( ; ; ) {
			int id = (((red > node->_mid_red ? 1 : 0) << 0) | ((green > node->_mid_green ? 1 : 0) << 1) | ((blue  > node->_mid_blue  ? 1 : 0) << 2)  );

			if (node->_child[id] == NULL) {
				break;
			}

			node = node->_child[id];
		}

		if (_mode == QUICK_MATCH) {
			pixels[i] = node->_color_number;
		} else if (_mode == CLOSE_MATCH) {
			search->distance = INT_MAX;
			node->_parent->closestColor(red, green, blue, search);
			pixels[i] = search->color_number;
		}
	}

	delete search;
}

void Cube::GetPalette(uint32_t **pixels)
{
	*pixels = (uint32_t *)_colormap;
}

Quantization *Quantization::_instance = NULL;

Quantization::Quantization()
{
	SQUARES = new uint32_t[MAX_RGB+MAX_RGB+1];

	for (int i=-MAX_RGB; i<=MAX_RGB; i++) {
		SQUARES[i+MAX_RGB] = i*i;
	}

	SHIFT = new uint32_t[MAX_TREE_DEPTH+1];

	for (int i=0; i<MAX_TREE_DEPTH+1; ++i) {
		SHIFT[i] = 1<<(15-i);
	}
}

Quantization::~Quantization()
{
}

Quantization * Quantization::GetInstance()
{
	if (_instance == NULL) {
		_instance = new Quantization();
	}

	return _instance;
}

Cube * Quantization::quantizeImage(uint32_t *pixels, int pixels_size, int max_colors) 
{
	Cube *cube = new Cube(pixels, pixels_size, max_colors);

	cube->classification();
	cube->reduction();
	cube->assignment();

	return cube;
}

