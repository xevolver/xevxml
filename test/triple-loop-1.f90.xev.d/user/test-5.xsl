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


	<xsl:template match="*" mode="loop_collapse">
		<xsl:param name="firstLoop" />
		<xsl:param name="secondLoop" />
		<xsl:apply-templates select="."
			mode="loop_collapse_find_first">
			<xsl:with-param name="firstLoop" select="$firstLoop" />
			<xsl:with-param name="secondLoop" select="$secondLoop" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template name="emitSgFortranDo">
		<xsl:comment>
			emitSgFortranDo
		</xsl:comment>
	</xsl:template>

	<xsl:template match="*" mode="loop_collapse_find_first">
		<xsl:param name="firstLoop" />
		<xsl:param name="secondLoop" />
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $firstLoop">
				<!-- get info from first loop -->
				<xsl:variable name="max" select="./*[2]" />
				<!-- <xsl:call-template name="emitSgFortranDo"></xsl:call-template> -->
				<xsl:apply-templates select="."
					mode="loop_collapse_find_second">
					<xsl:with-param name="firstMax" select="$max" />
					<xsl:with-param name="secondLoop" select="$secondLoop" />
				</xsl:apply-templates>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="loop_collapse_find_first">
						<xsl:with-param name="firstLoop" select="$firstLoop" />
						<xsl:with-param name="secondLoop" select="$secondLoop" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="*" mode="loop_collapse_find_second">
		<xsl:param name="firstMax" />
		<xsl:param name="secondLoop" />
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $secondLoop">
				<!--change max -->
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<!-- 変数 初期値 -->
					<xsl:copy-of select="./*[1]" />
					<!-- TODO change the last value -->
					<xsl:copy-of select="./*[2]" />
					<!-- TODO calculate the stride -->
					<xsl:element name="SgIntVal">
						<xsl:attribute name="value">
							<xsl:value-of select="$max" />
						</xsl:attribute>
					</xsl:element>
				</xsl:copy>

				<xsl:apply-templates select="./SgBasicBlock"
					mode="loop_collapse_body">
					<xsl:with-param name="max" select="$max" />
					<xsl:with-param name="var" select="$var" />
				</xsl:apply-templates>

			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="loop_collapse_find_second">
						<xsl:with-param name="firstMax" select="$firstMax" />
						<xsl:with-param name="secondLoop" select="$secondLoop" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="SgPragmaDeclaration" mode="loop_collapse">
	</xsl:template>
	<xsl:template match="PreprocessingInfo" mode="loop_collapse">
	</xsl:template>

</xsl:stylesheet>
	