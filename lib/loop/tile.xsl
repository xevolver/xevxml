<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevLoopTile">
		<xsl:param name="loopName" />
		<xsl:param name="start" />
		<xsl:param name="end" />
		<xsl:param name="size" />

		<!-- emit loop -->
		DO
		<xsl:value-of select="$loopName" />
		_tile =
		<xsl:value-of select="$start" />
		,
		<xsl:value-of select="$end" />
		,
		<xsl:value-of select="$size" />
		<xsl:apply-templates select="." mode="loop_tile_find_loop">
			<xsl:with-param name="loopName" select="$loopName" />
			<xsl:with-param name="end" select="$end" />
			<xsl:with-param name="size" select="$size" />
		</xsl:apply-templates>
		END DO
	</xsl:template>

	<xsl:template match="*" mode="loop_tile_find_loop">
		<xsl:param name="loopName" />
		<xsl:param name="end" />
		<xsl:param name="size" />
		<xsl:variable name="currentNode" select="." /> <!-- for debug -->
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $loopName">
				<xsl:comment>
					found loop
				</xsl:comment>

				DO
				<xsl:value-of select="$loopName" />
				=
				<xsl:value-of select="$loopName" />
				_tile, min(
				<xsl:value-of select="$end" />
				,
				<xsl:value-of select="$loopName" />
				_tile +
				<xsl:value-of select="$size" />
				- 1)
				<xsl:apply-templates select="self::SgFortranDo/SgBasicBlock">
				</xsl:apply-templates>
				END DO
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="loop_tile_find_loop">
						<xsl:with-param name="loopName" select="$loopName" />
						<xsl:with-param name="end" select="$end" />
						<xsl:with-param name="size" select="$size" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- <xsl:template match="SgPragmaDeclaration" mode="loop_tile"> </xsl:template> 
		<xsl:template match="PreprocessingInfo" mode="loop_tile"> </xsl:template> 
		<xsl:template match="SgPragmaDeclaration" mode="loop_tile_find_loop"> </xsl:template> 
		<xsl:template match="PreprocessingInfo" mode="loop_tile_find_loop"> </xsl:template> -->
</xsl:stylesheet>
