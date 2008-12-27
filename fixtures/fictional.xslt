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
		
		tenets:
		- you can always get an accurate simple refinement, if you have a consistent scope
		- simple refinement +c counter
		
		
		- you can always get an accurate simple refinement of the current scope,
													which you can use to for-each
		- complex refinements are handled with multikeys
		
		
		every scope is a completely independent (multi?)key, same countdown, even for simple
		- what do you need to carry?!
		match=parent-match//current-match
		use=parent-criteria-current-criteria
		for-each:parent-scope//current-key
		
		{
			"page": [{
				"title": "h1",
				"nav": ["ul#nav li"],
				"post(#posts li)": [{
					"title": "h2",
					"paras": ["./p"],
					"comment": [{
						"title": "#comments h3",
						"contents": ["#comments p"]
					}]
				}]
			}]
		}
	-->

	<xsl:template match="text()" mode="innertext">
		<xsl:value-of select="." />
	</xsl:template>

	<xsl:template match="br" mode="innertext">
		<apply-templates mode="innertext" />
		<xsl:text>
</xsl:text>
	</xsl:template>

	<xsl:template match="p" mode="innertext">
		<apply-templates mode="innertext" />
		<xsl:text>

</xsl:text>
	</xsl:template>
	
	<xsl:template match="/">
		<xsl:variable name="context0" select="." />
		<dexter:root>
			<page>
				<xsl:for-each select="//h1">
					<dexter:grp>
						<xsl:variable name="page_index" select="count(set:intersection(following::*, //h1))" />
						<title><xsl:value-of select="key('page-key', $page_index)/h1" /></title>
						<nav>
							<xsl:for-each select="key('page-key', $page_index)/ul[@id='nav']//li">
								<dexter:grp>
									<xsl:value-of select="." />
								</dexter:grp>
							</xsl:for-each>
						</nav>
						<post>
							<xsl:for-each select="key('page-key', $page_index)/*[@id='posts']//li">
								<xsl:variable name="context1" select="." />
								<xsl:variable name="post_index" select="concat($page_index, '-', count(set:intersection(following::*, //*[@id='posts']//li//h2)))" />
								<dexter:grp>
									<title><xsl:value-of select="key('post-key', $post_index)/h2" /></title>
									<paras>
										<xsl:for-each select="set:intersection(key('post-key', $post_index), exsl:node-set($context1)/p)">
											<dexter:grp>
												<xsl:value-of select="." />
											</dexter:grp>
										</xsl:for-each>
									</paras>
									<comment>
										<xsl:for-each select="key('post-key', $post_index)/*[@id='comments']//h3">
											<xsl:variable name="comment_index" select="concat($post_index, '-', count(set:intersection(following::*, //*[@id='posts']//li//*[@id='comments']//h3)))" />
											<dexter:grp>
												<title><xsl:value-of select="set:intersection(exsl:node-set($context1)//*[@id='comments']//h3, key('comment-key', $comment_index))" /></title>
												<contents>
													<xsl:for-each select="set:intersection(exsl:node-set($context1)//*[@id='comments']//p, key('comment-key', $comment_index))">
														<dexter:grp>
															<xsl:value-of select="." />
														</dexter:grp>
													</xsl:for-each>
												</contents>
											</dexter:grp>
										</xsl:for-each>
									</comment>
								</dexter:grp>
							</xsl:for-each>
						</post>
					</dexter:grp>
				</xsl:for-each>
			</page>
		</dexter:root>
		<xsl:apply-templates select="key('comment-key', '1-0-1')"  mode="debug"/>
	</xsl:template>
	
	<xsl:template match="@*|node()" mode="debug">
	  <xsl:copy>
	    <xsl:apply-templates select="@*|node()" mode="debug"/>
	  </xsl:copy>
	</xsl:template>
	
	<xsl:key name="page-key" match="//*" use="count(set:intersection(following::*, //h1))" />
	<xsl:key name="post-key" match="//*[@id='posts']//li/descendant-or-self::*" 
		use="concat(count(set:intersection(following::*, //h1)) , '-', count(set:intersection(following::*, //*[@id='posts']//li//h2)))" />
	<xsl:key name="comment-key" match="//*[@id='posts']//li/descendant-or-self::*" 
		use="concat(count(set:intersection(following::*, 4)), '-',
		            count(set:intersection(following::*, //*[@id='posts']//li//h2)), '-',
								count(set:intersection(following::*, //*[@id='posts']//li//*[@id='comments']//h3)))" />
			
</xsl:stylesheet>





