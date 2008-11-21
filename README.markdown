### Overview

Dexter is a simple language for data-extraction from XML-like documents (including HTML).  Dexter is:

1. Blazing fast -- Typical HTML parses are sub-50ms.
2. Easy to write and understand.
3. Powerful.  Dexter can understand full XPath, including standard and user-defined functions.

### Examples

A simple script, or "dex", looks like this:

    {
      "title": "h1",
      "links": [
        {
          "text": "a",
          "href": "$text/@href"
        }
      ]
    }

This returns JSON or XML output with the same structure.  Applying this dex to http://www.yelp.com/biz/amnesia-san-francisco yields either:

    {
      "title": "Amnesia",
      "links": [
        {
          "href": "\/",
          "text": "Yelp"
        },
        {
          "href": "\/",
          "text": "Welcome"
        },
        {
          "href": "\/signup?return_url=%2Fuser_details",
          "text": " About Me"
        },
        .....
      ]
    }

or equivalently:
  
    <root>
      <title>Amnesia</title>
      <links>
        <group>
          <href>/</href>
          <text>Yelp</text>
        </group>
        <group>
          <href>/</href>
          <text>Welcome</text>
        </group>
        <group>
          <href>/signup?return_url=%2Fuser_details</href>
          <text> About Me</text>
        </group>
        .....
      </links>
    </root>      

This dex could also have been expressed as:

    {
      "title": "h1",
      "links(a)": [
        {
          "text": ".",
          "href": "@href"
        }
      ]
    }

The "a" in links(a) is a "key selector" -- an explicit grouping (with scope) for the array.  You can use any XPath 1.0 or CSS3 expression as a value or a key selector.  Dexter will try to be smart, and figure out which you are using.  You can use CSS selectors inside XPath functions -- "substring-after(h1>a, ':')" is a valid expression.

### Variables

You can use $foo to access the value of the key "foo" in the current scope (i.e. nested curly brace depth).  Also available are $parent.foo, $parent.parent.foo, $root.foo, $root.foo.bar, etc.

### Custom Functions 

You can write custom functions in XSLT (I'd like to also support C and JavaScript).  They look like:

    <func:function name="user:excited">
       <xsl:param name="input" />
       <func:result select="concat($input, '!!!!!!!')" />
    </func:function>
    
If you run:

    {
      "title": "user:excited(h1)",
    }
    
on the Yelp page, you'll get:

    {
      "title": "Amnesia!!!!!!!",
    }
    