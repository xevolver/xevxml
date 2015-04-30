<?xml version="1.0" encoding="UTF-8"?>

<!-- Xev standard loop translation library -->

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="collapse.xsl" />
	<xsl:import href="fission.xsl" />
	<xsl:import href="fusion.xsl" />
	<xsl:import href="interchange.xsl" />
	<xsl:import href="inversion.xsl" />
	<xsl:import href="reversal.xsl" />
	<xsl:import href="tile.xsl" />
	<xsl:import href="unroll.xsl" />
	<xsl:import href="unswitching.xsl" />

	<!-- Undo transformations -->
	<xsl:import href="tile_undo.xsl" />
	<xsl:import href="unroll_undo.xsl" />

</xsl:stylesheet>
