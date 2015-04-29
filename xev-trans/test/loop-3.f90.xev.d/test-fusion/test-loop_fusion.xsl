<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="*" mode="xevInitHook">
		<xsl:apply-templates select="." mode="xevFindDirective">
			<xsl:with-param name="directiveName" select="'xev loop_tag'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevMoveHook">
		<xsl:apply-templates select=".." mode="xevSkipToNthLoop">
			<xsl:with-param name="loopName" select="'i'" />
			<xsl:with-param name="N" select="2" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevTransformationHook">
		<xsl:variable name="currentNode" select="." /> <!-- for debug -->
		<xsl:apply-templates select="." mode="xevLoopFusion" />
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="bkup">
		<xsl:choose> <!-- skip first loop -->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					xev loop_tag loop_fusion skip
				</xsl:comment>
			</xsl:when>
			<xsl:when test="preceding-sibling::*[2]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					xev loop_tag loop_fusion
				</xsl:comment>
				<xsl:apply-templates select="." mode="loop_fusion">
				</xsl:apply-templates>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates />
				</xsl:copy>
			</xsl:otherwise>

		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>
	