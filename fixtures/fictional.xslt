<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:dexter="http://kylemaxwell.com/dexter" xmlns:str="http://exslt.org/strings" xmlns:set="http://exslt.org/sets" xmlns:math="http://exslt.org/math" xmlns:func="http://exslt.org/functions" xmlns:user="http://kylemaxwell.com/dexter/user-functions" xmlns:dyn="http://exslt.org/dynamic" xmlns:date="http://exslt.org/dates-and-times" xmlns:exsl="http://exslt.org/common" xmlns:saxon="http://icl.com/saxon" version="1.0" extension-element-prefixes="str math set func dyn exsl saxon user date">
	
	<xsl:output method="xml" indent="yes"/>
	<xsl:strip-space elements="*"/>
	
	<!-- 	
		{
			"page": [{
				"title": "h1",
				"nav": ["ul#nav li"],
				"post(#posts li)": [{
					"title": "h2",
					"paras": ["p"],
					"comments": [{
						"title": "#comments h3",
						"paras": ["#comments p"]
					}]
				}]
			}]
		}
	-->
	
	<xsl:template match="/">
		<dexter:root xmlns:dexter="http://kylemaxwell.com/dexter">
		  <page>
		    <dexter:groups>
		      <xsl:for-each select="//h1">
						<xsl:variable name="index1" select="last() - position()" />
		        <dexter:group>
		          <title><xsl:value-of select="." /></title>
		          <nav>
		            <dexter:groups>
		              <xsl:for-each select="set:intersection(key('key1', $index1), //ul[@id='nav']//li)">
		                <dexter:group>
		                	<xsl:value-of select="." />	
		                </dexter:group>
		              </xsl:for-each>
		            </dexter:groups>
		          </nav>
		          <post>
		            <dexter:groups>
		              <xsl:for-each select="set:intersection(key('key1', $index1), //*[@id='posts']//li)">
										<xsl:variable name="post" value="position()" />
		                <dexter:group>
		                  <title><xsl:value-of select=".//h2" /></title>
		                  <paras>
		                    <dexter:groups>
		                      <xsl:for-each select="./p">
		                        <dexter:group><xsl:value-of select="."/></dexter:group>
		                      </xsl:for-each>
		                    </dexter:groups>
		                  </paras>
		                  <comments>
		                    <dexter:groups>
		                      <xsl:for-each select=".//*[@id='comments']//h3">
														<xsl:variable name="index2" select="$index1 + (last() - position())" />
		                        <dexter:group>
		                          <title><xsl:value-of select="."/></title>
		                          <paras>
		                            <dexter:groups>
																		<xsl:for-each select="set:intersection(key('key2', $index2), //p)">
		                                <dexter:group><xsl:value-of select="."/></dexter:group>
		                              </xsl:for-each>
		                            </dexter:groups>
		                          </paras>
		                        </dexter:group>
		                      </xsl:for-each>
		                    </dexter:groups>
		                  </comments>
		                </dexter:group>
		              </xsl:for-each>
		            </dexter:groups>
		          </post>
		        </dexter:group>
		      </xsl:for-each>
		    </dexter:groups>
		  </page>
		</dexter:root>
	</xsl:template>
	
	<xsl:key name="key1" match="//*" use="count(following::h1)" />
	<xsl:key name="key2" match="//*" use="count(following::*[@id='comments']//h3)" />
			
</xsl:stylesheet>





