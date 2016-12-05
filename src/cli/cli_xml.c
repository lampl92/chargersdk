#include "includes.h"
#include "mxml.h"

static void cli_testxml_create_fnt(int argc, char **argv)
{
    FIL fp;
    int i;
    //mxml_node_t *tree;
    mxml_node_t *xml;    /* <?xml ... ?> */
    mxml_node_t *data;   /* <data> */
    mxml_node_t *node;   /* <node> */
    mxml_node_t *group;  /* <group> */

    xml = mxmlNewXML("1.0");

    data = mxmlNewElement(xml, "data");
    for (i = 0; i < 100; i++)
    {
    node = mxmlNewElement(data, "node");
    mxmlNewText(node, 0, "val1");
    node = mxmlNewElement(data, "node");
    mxmlNewText(node, 0, "val2");
    node = mxmlNewElement(data, "node");
    mxmlNewText(node, 0, "val3");

    group = mxmlNewElement(data, "group");

    node = mxmlNewElement(group, "node");
    mxmlNewText(node, 0, "val4");
    node = mxmlNewElement(group, "node");
    mxmlNewText(node, 0, "val5");
    node = mxmlNewElement(group, "node");
    mxmlNewText(node, 0, "val6");

    node = mxmlNewElement(data, "node");
    mxmlNewText(node, 0, "val7");
    node = mxmlNewElement(data, "node");
    mxmlNewText(node, 0, "val8");
if(i%10 == 0)
    printf("i = %d\n",i);
}

    f_open(&fp,"testmxml.xml", FA_CREATE_ALWAYS | FA_WRITE);
    mxmlSaveFile(xml, &fp, MXML_NO_CALLBACK);
    f_close(&fp);
    mxmlDelete(xml);
    
}


static void cli_testxml_fnt(int argc, char **argv)
{

}

tinysh_cmd_t cli_testxml_cmd = {0, "testxml", "xml½âÎö²âÊÔ", "[args]",
                                cli_testxml_fnt, 0, 0, 0
                               };

tinysh_cmd_t cli_testxml_create_cmd = {0, "testxmlcreate", "xml´´½¨²âÊÔ", "[args]",
                                       cli_testxml_create_fnt, 0, 0, 0
                                      };
