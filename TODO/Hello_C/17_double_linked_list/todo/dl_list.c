// dl_list.c
/*
  double linked list

  The main difference between a single and double linked list consists in
  more work on the linking, of course. But it is much easier to insert or
  remove elements in the middle of the list now, because a double linked
  list allows iterating upwards AND DOWNWARDS (which is a big advantage)!!!

  E.g. when removing an element, it is not necessary additionaly to run
  through all elements to figure out the element before in order to reset
  the linkage properly. Now we can simply use the prev-link of the element
  directly.
//*/

// TODO

// internal usage
static element_t *find(const char *, const unsigned int);
static int append(element_t *);
static int discard(element_t **);
static element_t *getnew();
static int init(element_t *, const char *, const unsigned int);

// first and last pointer
static element_t *first = NULL;
static element_t *last = NULL;

/*
  removes the whole list
//*/
int removeall()
{
	// TODO
	return 0;
}

/*
  appends an element to the end of the list
    data       data of the element to append
    data_size  size of data
//*/
int appendelement(const char *data, const unsigned int data_size)
{
	// TODO
	return 0;
}

/*
  inserts an element after a given element (prev_data)

    prev_data       content of the previous element
    data            content of the new element
    data_size       size of data
//*/
int insertelement(const char *prev_data, const unsigned int prev_data_size,
		  const char *data, const unsigned int data_size)
{
	// TODO
	return 0;
}

/*
  removes an element in the list

    data       data of the element to find
    data_size  size of data
//*/
// FIXME: cut's of elements before!!!
int removeelement(const char *data, const unsigned int data_size)
{
	// TODO
	return 0;
}

/* ******************************************************************************

  the following section are functions internally used in the list

//
******************************************************************************/

/*
  prints the content of the whole list
//*/
int printlist()
{
	// TODO
	return 0;
}

/*
  find an element by its data

    data       data of the element to find
    data_size  size of data

    returns a pointer to the element containing data or NULL
//*/
static element_t *find(const char *data, const unsigned int data_size)
{
	// TODO
	return NULL;
}

/*
  appends an element to the list
    tmp            element to append
//*/
static int append(element_t *tmp)
{
	// TODO
	return 0;
}

/*
  removes and frees the space of an element
    tmp       the elment to remove
//*/
static int discard(element_t **tmp)
{
	// TODO
	return 0;
}

/*
  allocates space for a new element

  returns a pointer to the new element
//*/
static element_t *getnew()
{
	element_t *tmp = NULL;

	// TODO

	return tmp;
}

/*
  init the value of an element
    tmp          element
    data         data value
    data_size    size of data
//*/
static int init(element_t *tmp, const char *data, const unsigned int data_size)
{
	// TODO
	return 0;
}
