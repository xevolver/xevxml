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
		<xsl:apply-templates select="." mode="xevGoToLoop">
			<xsl:with-param name="loopName" select="'j1'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevTransformationHook">
		<xsl:apply-templates select="." mode="xevLoopTileUndo">
			<xsl:with-param name="loopName1" select="'j'" />
			<xsl:with-param name="start1" select="'1'" />
			<xsl:with-param name="end1" select="'N'" />
			<xsl:with-param name="loopName2" select="'i'" />
			<xsl:with-param name="start2" select="'1'" />
			<xsl:with-param name="end2" select="'N'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="bkup">
		<xsl:choose>
			<xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					xev loop_tag loop_tile_undo
				</xsl:comment>
				<xsl:apply-templates select="." mode="loop_tile_undo">
					<xsl:with-param name="loopName1" select="'j'" />
					<xsl:with-param name="start1" select="'1'" />
					<xsl:with-param name="end1" select="'N'" />
					<xsl:with-param name="loopName2" select="'i'" />
					<xsl:with-param name="start2" select="'1'" />
					<xsl:with-param name="end2" select="'N'" />
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
	