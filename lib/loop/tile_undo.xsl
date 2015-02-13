<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="loop_tile_undo">
		<xsl:param name="loopName1" />
		<xsl:param name="start1" />
		<xsl:param name="end1" />
		<xsl:param name="loopName2" />
		<xsl:param name="start2" />
		<xsl:param name="end2" />

		<xsl:choose>
			<xsl:when test="self::SgFortranDo">
				<xsl:choose>
					<xsl:when
						test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $loopName1">
						<!-- emit loop2 -->
						DO						<xsl:value-of select="$loopName1" />						=						<xsl:value-of select="$start1" />						,						<xsl:value-of select="$end1" />
						<xsl:apply-templates select="./SgBasicBlock"
							mode="loop_tile_undo">
							<xsl:with-param name="loopName1" select="$loopName1" />
							<xsl:with-param name="start1" select="$start1" />
							<xsl:with-param name="end1" select="$end1" />
							<xsl:with-param name="loopName2" select="$loopName2" />
							<xsl:with-param name="start2" select="$start2" />
							<xsl:with-param name="end2" select="$end2" />
						</xsl:apply-templates>
						END DO
					</xsl:when>
					<xsl:when
						test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $loopName2">
						<!-- emit loop2 -->
						DO						<xsl:value-of select="$loopName2" />						=						<xsl:value-of select="$start2" />						,						<xsl:value-of select="$end2" />
						<xsl:apply-templates select="./SgBasicBlock"
							mode="loop_tile_undo">
							<xsl:with-param name="loopName1" select="$loopName1" />
							<xsl:with-param name="start1" select="$start1" />
							<xsl:with-param name="end1" select="$end1" />
							<xsl:with-param name="loopName2" select="$loopName2" />
							<xsl:with-param name="start2" select="$start2" />
							<xsl:with-param name="end2" select="$end2" />
						</xsl:apply-templates>
						END DO
					</xsl:when>
					<xsl:otherwise>
						<xsl:apply-templates select="./SgBasicBlock"
							mode="loop_tile_undo">
							<xsl:with-param name="loopName1" select="$loopName1" />
							<xsl:with-param name="start1" select="$start1" />
							<xsl:with-param name="end1" select="$end1" />
							<xsl:with-param name="loopName2" select="$loopName2" />
							<xsl:with-param name="start2" select="$start2" />
							<xsl:with-param name="end2" select="$end2" />
						</xsl:apply-templates>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="loop_tile_undo">
						<xsl:with-param name="loopName1" select="$loopName1" />
						<xsl:with-param name="start1" select="$start1" />
						<xsl:with-param name="end1" select="$end1" />
						<xsl:with-param name="loopName2" select="$loopName2" />
						<xsl:with-param name="start2" select="$start2" />
						<xsl:with-param name="end2" select="$end2" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>

	</xsl:template>

	<xsl:template match="SgPragmaDeclaration" mode="loop_tile_undo">
	</xsl:template>
	<xsl:template match="PreprocessingInfo" mode="loop_tile_undo">
	</xsl:template>

</xsl:stylesheet>