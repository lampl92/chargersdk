/*
 * "$Id: testmxml.c 466 2016-06-13 00:27:11Z msweet $"
 *
 * Test program for Mini-XML, a small XML-like file parsing library.
 *
 * Usage:
 *
 *   ./testmxml input.xml [string-output.xml] >stdio-output.xml
 *   ./testmxml "<?xml ..." [string-output.xml] >stdio-output.xml
 *
 * Copyright 2003-2016 by Michael R Sweet.
 *
 * These coded instructions, statements, and computer programs are the
 * property of Michael R Sweet and are protected by Federal copyright
 * law.  Distribution and use rights are outlined in the file "COPYING"
 * which should have been included with this file.  If this file is
 * missing or damaged, see the license at:
 *
 *     http://www.msweet.org/projects.php/Mini-XML
 */

/*
 * Include necessary headers...
 */

#include "config.h"
#include "mxml.h"

/*
 * Globals...
 */

int		event_counts[6];


/*
 * 'main()' - Main entry for test program.
 */

int					/* O - Exit status */
testmxml_entry(int  argc,				/* I - Number of command-line args */
     char *argv[])			/* I - Command-line args */
{
  int			i;		/* Looping var */
  FIL			fil;		/* File to read */
  mxml_node_t		*tree,		/* XML tree */
			*node;		/* Node which should be in test.xml */
  mxml_index_t		*ind;		/* XML index */
  char			buffer[16384];	/* Save string */
  static const char	*types[] =	/* Strings for node types */
			{
			  "MXML_ELEMENT",
			  "MXML_INTEGER",
			  "MXML_OPAQUE",
			  "MXML_REAL",
			  "MXML_TEXT"
			};


 /*
  * Check arguments...
  */

  if (argc != 2 && argc != 3)
  {
    fputs("Usage: testmxml filename.xml [string-output.xml]\n", stderr);
    return (1);
  }

 /*
  * Test the basic functionality...
  */

  tree = mxmlNewElement(MXML_NO_PARENT, "element");

  if (!tree)
  {
    fputs("ERROR: No parent node in basic test.\n", stderr);
    return (1);
  }

  if (tree->type != MXML_ELEMENT)
  {
    fprintf(stderr, "ERROR: Parent has type %s (%d), expected MXML_ELEMENT.\n",
            tree->type < MXML_ELEMENT || tree->type > MXML_TEXT ?
	        "UNKNOWN" : types[tree->type], tree->type);
    mxmlDelete(tree);
    return (1);
  }

  if (strcmp(tree->value.element.name, "element"))
  {
    fprintf(stderr, "ERROR: Parent value is \"%s\", expected \"element\".\n",
            tree->value.element.name);
    mxmlDelete(tree);
    return (1);
  }

  mxmlNewInteger(tree, 123);
  mxmlNewOpaque(tree, "opaque");
  mxmlNewReal(tree, 123.4f);
  mxmlNewText(tree, 1, "text");

  mxmlLoadString(tree, "<group type='string'>string string string</group>",
                 MXML_NO_CALLBACK);
  mxmlLoadString(tree, "<group type='integer'>1 2 3</group>",
                 MXML_INTEGER_CALLBACK);
  mxmlLoadString(tree, "<group type='real'>1.0 2.0 3.0</group>",
                 MXML_REAL_CALLBACK);
  mxmlLoadString(tree, "<group>opaque opaque opaque</group>",
                 MXML_OPAQUE_CALLBACK);
  mxmlLoadString(tree, "<foo><bar><one><two>value<two>value2</two></two></one>"
                       "</bar></foo>", MXML_OPAQUE_CALLBACK);

  node = tree->child;

  if (!node)
  {
    fputs("ERROR: No first child node in basic test.\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

  if (node->type != MXML_INTEGER)
  {
    fprintf(stderr, "ERROR: First child has type %s (%d), expected MXML_INTEGER.\n",
            node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
	        "UNKNOWN" : types[node->type], node->type);
    mxmlDelete(tree);
    return (1);
  }

  if (node->value.integer != 123)
  {
    fprintf(stderr, "ERROR: First child value is %d, expected 123.\n",
            node->value.integer);
    mxmlDelete(tree);
    return (1);
  }

  node = node->next;

  if (!node)
  {
    fputs("ERROR: No second child node in basic test.\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

  if (node->type != MXML_OPAQUE)
  {
    fprintf(stderr, "ERROR: Second child has type %s (%d), expected MXML_OPAQUE.\n",
            node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
	        "UNKNOWN" : types[node->type], node->type);
    mxmlDelete(tree);
    return (1);
  }

  if (!node->value.opaque || strcmp(node->value.opaque, "opaque"))
  {
    fprintf(stderr, "ERROR: Second child value is \"%s\", expected \"opaque\".\n",
            node->value.opaque ? node->value.opaque : "(null)");
    mxmlDelete(tree);
    return (1);
  }

  node = node->next;

  if (!node)
  {
    fputs("ERROR: No third child node in basic test.\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

  if (node->type != MXML_REAL)
  {
    fprintf(stderr, "ERROR: Third child has type %s (%d), expected MXML_REAL.\n",
            node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
	        "UNKNOWN" : types[node->type], node->type);
    mxmlDelete(tree);
    return (1);
  }

  if (node->value.real != 123.4f)
  {
    fprintf(stderr, "ERROR: Third child value is %f, expected 123.4.\n",
            node->value.real);
    mxmlDelete(tree);
    return (1);
  }

  node = node->next;

  if (!node)
  {
    fputs("ERROR: No fourth child node in basic test.\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

  if (node->type != MXML_TEXT)
  {
    fprintf(stderr, "ERROR: Fourth child has type %s (%d), expected MXML_TEXT.\n",
            node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
	        "UNKNOWN" : types[node->type], node->type);
    mxmlDelete(tree);
    return (1);
  }

  if (!node->value.text.whitespace ||
      !node->value.text.string || strcmp(node->value.text.string, "text"))
  {
    fprintf(stderr, "ERROR: Fourth child value is %d,\"%s\", expected 1,\"text\".\n",
            node->value.text.whitespace,
	    node->value.text.string ? node->value.text.string : "(null)");
    mxmlDelete(tree);
    return (1);
  }

  for (i = 0; i < 4; i ++)
  {
    node = node->next;

    if (!node)
    {
      fprintf(stderr, "ERROR: No group #%d child node in basic test.\n", i + 1);
      mxmlDelete(tree);
      return (1);
    }

    if (node->type != MXML_ELEMENT)
    {
      fprintf(stderr, "ERROR: Group child #%d has type %s (%d), expected MXML_ELEMENT.\n",
              i + 1, node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
	                 "UNKNOWN" : types[node->type], node->type);
      mxmlDelete(tree);
      return (1);
    }
  }

 /*
  * Test mxmlFindPath...
  */

  node = mxmlFindPath(tree, "*/two");
  if (!node)
  {
    fputs("ERROR: Unable to find value for \"*/two\".\n", stderr);
    mxmlDelete(tree);
    return (1);
  }
  else if (node->type != MXML_OPAQUE || strcmp(node->value.opaque, "value"))
  {
    fputs("ERROR: Bad value for \"*/two\".\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

  node = mxmlFindPath(tree, "foo/*/two");
  if (!node)
  {
    fputs("ERROR: Unable to find value for \"foo/*/two\".\n", stderr);
    mxmlDelete(tree);
    return (1);
  }
  else if (node->type != MXML_OPAQUE || strcmp(node->value.opaque, "value"))
  {
    fputs("ERROR: Bad value for \"foo/*/two\".\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

  node = mxmlFindPath(tree, "foo/bar/one/two");
  if (!node)
  {
    fputs("ERROR: Unable to find value for \"foo/bar/one/two\".\n", stderr);
    mxmlDelete(tree);
    return (1);
  }
  else if (node->type != MXML_OPAQUE || strcmp(node->value.opaque, "value"))
  {
    fputs("ERROR: Bad value for \"foo/bar/one/two\".\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

 /*
  * Test indices...
  */

  ind = mxmlIndexNew(tree, NULL, NULL);
  if (!ind)
  {
    fputs("ERROR: Unable to create index of all nodes.\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

  if (ind->num_nodes != 10)
  {
    fprintf(stderr, "ERROR: Index of all nodes contains %d "
                    "nodes; expected 10.\n", ind->num_nodes);
    mxmlIndexDelete(ind);
    mxmlDelete(tree);
    return (1);
  }

  mxmlIndexReset(ind);
  if (!mxmlIndexFind(ind, "group", NULL))
  {
    fputs("ERROR: mxmlIndexFind for \"group\" failed.\n", stderr);
    mxmlIndexDelete(ind);
    mxmlDelete(tree);
    return (1);
  }

  mxmlIndexDelete(ind);

  ind = mxmlIndexNew(tree, "group", NULL);
  if (!ind)
  {
    fputs("ERROR: Unable to create index of groups.\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

  if (ind->num_nodes != 4)
  {
    fprintf(stderr, "ERROR: Index of groups contains %d "
                    "nodes; expected 4.\n", ind->num_nodes);
    mxmlIndexDelete(ind);
    mxmlDelete(tree);
    return (1);
  }

  mxmlIndexReset(ind);
  if (!mxmlIndexEnum(ind))
  {
    fputs("ERROR: mxmlIndexEnum failed.\n", stderr);
    mxmlIndexDelete(ind);
    mxmlDelete(tree);
    return (1);
  }

  mxmlIndexDelete(ind);

  ind = mxmlIndexNew(tree, NULL, "type");
  if (!ind)
  {
    fputs("ERROR: Unable to create index of type attributes.\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

  if (ind->num_nodes != 3)
  {
    fprintf(stderr, "ERROR: Index of type attributes contains %d "
                    "nodes; expected 3.\n", ind->num_nodes);
    mxmlIndexDelete(ind);
    mxmlDelete(tree);
    return (1);
  }

  mxmlIndexReset(ind);
  if (!mxmlIndexFind(ind, NULL, "string"))
  {
    fputs("ERROR: mxmlIndexFind for \"string\" failed.\n", stderr);
    mxmlIndexDelete(ind);
    mxmlDelete(tree);
    return (1);
  }

  mxmlIndexDelete(ind);

  ind = mxmlIndexNew(tree, "group", "type");
  if (!ind)
  {
    fputs("ERROR: Unable to create index of elements and attributes.\n", stderr);
    mxmlDelete(tree);
    return (1);
  }

  if (ind->num_nodes != 3)
  {
    fprintf(stderr, "ERROR: Index of elements and attributes contains %d "
                    "nodes; expected 3.\n", ind->num_nodes);
    mxmlIndexDelete(ind);
    mxmlDelete(tree);
    return (1);
  }

  mxmlIndexReset(ind);
  if (!mxmlIndexFind(ind, "group", "string"))
  {
    fputs("ERROR: mxmlIndexFind for \"string\" failed.\n", stderr);
    mxmlIndexDelete(ind);
    mxmlDelete(tree);
    return (1);
  }

  mxmlIndexDelete(ind);

 /*
  * Check the mxmlDelete() works properly...
  */

  for (i = 0; i < 9; i ++)
  {
    if (tree->child)
      mxmlDelete(tree->child);
    else
    {
      fprintf(stderr, "ERROR: Child pointer prematurely NULL on child #%d\n",
              i + 1);
      mxmlDelete(tree);
      return (1);
    }
  }

  if (tree->child)
  {
    fputs("ERROR: Child pointer not NULL after deleting all children.\n", stderr);
    return (1);
  }

  if (tree->last_child)
  {
    fputs("ERROR: Last child pointer not NULL after deleting all children.\n", stderr);
    return (1);
  }

  mxmlDelete(tree);

 /*
  * Open the file/string using the default (MXML_NO_CALLBACK) callback...
  */

  if (argv[1][0] == '<')
    tree = mxmlLoadString(NULL, argv[1], MXML_NO_CALLBACK);
  else if ((f_open(&fil, argv[1], FA_READ)) != FR_OK)
  {
    perror(argv[1]);
    return (1);
  }
  else
  {
   /*
    * Read the file...
    */

    tree = mxmlLoadFile(NULL, &fil, MXML_NO_CALLBACK);

    f_close(&fil);
  }

  if (!tree)
  {
    fputs("Unable to read XML file with default callback.\n", stderr);
    return (1);
  }

  if (!strcmp(argv[1], "test.xml"))
  {
    const char	*text;			/* Text value */

   /*
    * Verify that mxmlFindElement() and indirectly mxmlWalkNext() work
    * properly...
    */

    if ((node = mxmlFindPath(tree, "group/option/keyword")) == NULL)
    {
      fputs("Unable to find group/option/keyword element in XML tree.\n", stderr);
      mxmlDelete(tree);
      return (1);
    }

    if (node->type != MXML_TEXT)
    {
      fputs("No child node of group/option/keyword.\n", stderr);
        
      //mxmlSaveFile(tree, &fstderr2xml, MXML_NO_CALLBACK);
      
      mxmlDelete(tree);
      return (1);
    }

    if ((text = mxmlGetText(node, NULL)) == NULL || strcmp(text, "InputSlot"))
    {
      fprintf(stderr, "Child node of group/option/value has value \"%s\" instead of \"InputSlot\".\n", text ? text : "(null)");
      mxmlDelete(tree);
      return (1);
    }
  }

  mxmlDelete(tree);

 /*
  * Open the file...
  */

  if (argv[1][0] == '<')
    tree = mxmlLoadString(NULL, argv[1], type_cb);
  else if (f_open(&fil, argv[1], FA_READ) != FR_OK)
  {
    perror(argv[1]);
    return (1);
  }
  else
  {
   /*
    * Read the file...
    */

    tree = mxmlLoadFile(NULL, &fil, type_cb);

    f_close(&fil);
  }

  if (!tree)
  {
    fputs("Unable to read XML file.\n", stderr);
    return (1);
  }

  if (!strcmp(argv[1], "test.xml"))
  {
   /*
    * Verify that mxmlFindElement() and indirectly mxmlWalkNext() work
    * properly...
    */

    if ((node = mxmlFindElement(tree, tree, "choice", NULL, NULL,
                                MXML_DESCEND)) == NULL)
    {
      fputs("Unable to find first <choice> element in XML tree.\n", stderr);
      mxmlDelete(tree);
      return (1);
    }

    if (!mxmlFindElement(node, tree, "choice", NULL, NULL, MXML_NO_DESCEND))
    {
      fputs("Unable to find second <choice> element in XML tree.\n", stderr);
      mxmlDelete(tree);
      return (1);
    }
  }

 /*
  * Print the XML tree...
  */
  //mxmlSaveFile(tree, stdout, whitespace_cb);
 /*
  * Save the XML tree to a string and print it...
  */

  if (mxmlSaveString(tree, buffer, sizeof(buffer), whitespace_cb) > 0)
  {
    if (argc == 3)
    {
      f_open(&fil, argv[2], FA_OPEN_ALWAYS | FA_WRITE);
      f_puts(buffer, &fil);
      f_close(&fil);
      printf("%s", buffer);
    }
  }

 /*
  * Delete the tree...
  */

  mxmlDelete(tree);

 /*
  * Test SAX methods...
  */

  memset(event_counts, 0, sizeof(event_counts));

  if (argv[1][0] == '<')
    mxmlSAXLoadString(NULL, argv[1], type_cb, sax_cb, NULL);
  else if ((f_open(&fil, argv[1], FA_READ)) != FR_OK)
  {
    perror(argv[1]);
    return (1);
  }
  else
  {
   /*
    * Read the file...
    */

    mxmlSAXLoadFile(NULL, &fil, type_cb, sax_cb, NULL);

    f_close(&fil);
  }

  if (!strcmp(argv[1], "test.xml"))
  {
    if (event_counts[MXML_SAX_CDATA] != 1)
    {
      fprintf(stderr, "MXML_SAX_CDATA seen %d times, expected 1 times.\n",
              event_counts[MXML_SAX_CDATA]);
      return (1);
    }

    if (event_counts[MXML_SAX_COMMENT] != 1)
    {
      fprintf(stderr, "MXML_SAX_COMMENT seen %d times, expected 1 times.\n",
              event_counts[MXML_SAX_COMMENT]);
      return (1);
    }

    if (event_counts[MXML_SAX_DATA] != 60)
    {
      fprintf(stderr, "MXML_SAX_DATA seen %d times, expected 60 times.\n",
              event_counts[MXML_SAX_DATA]);
      return (1);
    }

    if (event_counts[MXML_SAX_DIRECTIVE] != 1)
    {
      fprintf(stderr, "MXML_SAX_DIRECTIVE seen %d times, expected 1 times.\n",
              event_counts[MXML_SAX_DIRECTIVE]);
      return (1);
    }

    if (event_counts[MXML_SAX_ELEMENT_CLOSE] != 20)
    {
      fprintf(stderr, "MXML_SAX_ELEMENT_CLOSE seen %d times, expected 20 times.\n",
              event_counts[MXML_SAX_ELEMENT_CLOSE]);
      return (1);
    }

    if (event_counts[MXML_SAX_ELEMENT_OPEN] != 20)
    {
      fprintf(stderr, "MXML_SAX_ELEMENT_OPEN seen %d times, expected 20 times.\n",
              event_counts[MXML_SAX_ELEMENT_OPEN]);
      return (1);
    }
  }

 /*
  * Return...
  */

  return (0);
}



/*
 * End of "$Id: testmxml.c 466 2016-06-13 00:27:11Z msweet $".
 */
