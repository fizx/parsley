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
					"subcomments": [{
						"title": "#comments h3",
						"tags": ["#comments p"]
					}]
				}]
			}]
		}
	-->
	
	<xsl:template match="/">
		<dexter:root>
			<page>
				<dexter:groups>
					<dexter:group>
						<title></title>
						<nav><dexter:groups><dexter:group></dexter:group></dexter:groups></nav>
						<post>
							<dexter:groups>
								<dexter:group>
									<title></title>
									<paras><dexter:groups><dexter:group></dexter:group></dexter:groups></paras>
									<subcomments>
										<dexter:groups>
											<dexter:group>
												<title></title>
												<tags><dexter:groups><dexter:group></dexter:group></dexter:groups></tags>
											</dexter:group>
										</dexter:groups>
									</subcomments>
								</dexter:group>
							</dexter:groups>
						</post>
					</dexter:group>
				</dexter:groups>
			</page>
		</dexter:root>
	</xsl:template>
			
</xsl:stylesheet>





