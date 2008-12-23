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
		
		key looks like: 			use="count(set:intersection(following::*, <KEY>))
		
		visitation:
		page
		- type: magic, key: //h1									
		page>title
		- type: simple
		- append-key: page-title-key
		- value-of:select key
		page>nav
		- type:simple-array
		- append-key: page-nav-key
		- for-each select(key) > value-of .
		
		
		
		<xsl:key name="comment-title-key" match="//*[@id='comments']//h3" use="count(set:intersection(following::*, //*[@id='comments']//h3))" />
		
		
		
		["page", :magic, "h1"] => {
		
		
		}
		
		
		
		]
	-->
	
	<xsl:template match="/">
		<dexter:root xmlns:dexter="http://kylemaxwell.com/dexter">
			<xsl:for-each select="//h1">
			  <page>
					<xsl:variable name="page_index" select="last() - position()" />
					<xsl:attribute name="title"><xsl:value-of select="key('page-key', $page_index)/h1" /></xsl:attribute>

					<xsl:for-each select="key('page-key', $page_index)/*[@id='posts']//li">
							<post>
								<xsl:attribute name="title"><xsl:value-of select=".//h2" /></xsl:attribute>
								<xsl:for-each select=".//*[@id='comments']//h3">
									<comment>
										<xsl:variable name="comment_index" select="count(set:intersection(following::*, //*[@id='posts']//li//*[@id='comments']//h3))"/>
										<xsl:attribute name="index"><xsl:value-of select="$comment_index" /></xsl:attribute>
										<xsl:attribute name="ele"><xsl:value-of select="count(key('comment-title-key', $comment_index))" /></xsl:attribute>
										<xsl:attribute name="title"><xsl:value-of select="key('`', $comment_index)" /></xsl:attribute>

										<xsl:for-each select="key('comment-paras-key', $comment_index)">
											<para><xsl:value-of select="."/></para>
										</xsl:for-each>
									</comment>
								</xsl:for-each>
							</post>
					</xsl:for-each>
				</page>
			</xsl:for-each>
		</dexter:root>
	</xsl:template>
	
	<xsl:template match="node()|@*">
	  <xsl:copy>
	    <xsl:apply-templates select="@*|node()"/>
	  </xsl:copy>
	</xsl:template>
	
	<xsl:key name="page-key" match="//*" use="count(following::h1)" />
	<!-- <xsl:key name="post-key" match="//*" use="." /> -->
	<xsl:key name="comment-title-key" match="//*[@id='comments']//h3" use="count(set:intersection(following::*, //*[@id='comments']//h3))" />
	<xsl:key name="comment-paras-key" match="//*[@id='comments']//p" use="count(set:intersection(following::*, //*[@id='comments']//h3))" />
			
</xsl:stylesheet>





