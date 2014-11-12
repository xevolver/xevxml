<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag'">
				<xsl:comment>
					test-5.xsl xev loop_tag
				</xsl:comment>

				<xsl:variable name="step1">
					<xsl:comment>
						test-5.xsl step1
					</xsl:comment>
					<xsl:apply-templates select="." mode="loop_collapse">
					</xsl:apply-templates>
				</xsl:variable>

				<!-- <xsl:apply-templates select="exslt:node-set($step1)" -->
				<!-- mode="chill_unroll_jam"> -->
				<!-- <xsl:with-param name="max" select="4" /> -->
				<!-- <xsl:with-param name="var" select="'k'" /> -->
				<xsl:apply-templates />

			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates />
				</xsl:copy>
			</xsl:otherwise>

		</xsl:choose>
	</xsl:template>


	<xsl:template match="*" mode="find_loop_and_unroll">
		<xsl:choose>
			<xsl:when test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = 'i'">
				<xsl:apply-templates select="." mode="chill_unroll">
					<xsl:with-param name="max" select="2" />
					<xsl:with-param name="var" select="'i'" />
				</xsl:apply-templates>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="find_loop_and_unroll" />
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="SgPragmaDeclaration" mode="find_loop_and_unroll">
	</xsl:template>
	<xsl:template match="PreprocessingInfo" mode="find_loop_and_unroll">
	</xsl:template>

</xsl:stylesheet>
	