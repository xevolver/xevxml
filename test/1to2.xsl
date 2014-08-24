<?xml version="1.0" encoding="shift_jis"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<!-- 関数呼出し時にprintfかどうかのフラグを設定する -->
  <xsl:template match="SgFunctionCallExp">
    <xsl:choose>
      <xsl:when test="./SgFunctionRefExp/@symbol='printf'">
	<xsl:copy>
	  <xsl:apply-templates>
	    <xsl:with-param name="isInPrintf" select="'yes'"/>
	  </xsl:apply-templates>
	</xsl:copy>
      </xsl:when>
      <xsl:otherwise>
	<xsl:copy>
	  <xsl:apply-templates select="node()|@*">
	    <xsl:with-param name="isInPrintf" select="'no'"/>
	  </xsl:apply-templates>
	</xsl:copy>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<!-- 関数呼出しの部分木中ではprintfフラグを伝搬する -->
  <xsl:template match="SgFunctionCallExp//*">
    <xsl:param name="isInPrintf"/>
    <xsl:copy>
      <xsl:apply-templates select="node()|@*">
	<xsl:with-param name="isInPrintf" select="$isInPrintf"/>
      </xsl:apply-templates>
    </xsl:copy>
  </xsl:template>

<!-- printfのフォーマット文字列を書き換え -->
  <xsl:template match="SgStringVal/@value">
    <xsl:param name="isInPrintf"/>
    <xsl:choose>
      <xsl:when test="$isInPrintf='yes'">
	<xsl:attribute name="value">
	  <xsl:value-of select="'Hello, Xevolver %d'"/>
	</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
	<xsl:copy-of select="."/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<!-- printfの引数に変数iを追加 -->
  <xsl:template match="SgFunctionCallExp/SgExprListExp">
    <xsl:param name="isInPrintf"/>
    <xsl:copy>
      <xsl:apply-templates select="node()|@*">
	<xsl:with-param name="isInPrintf" select="$isInPrintf"/>
      </xsl:apply-templates>
      <xsl:if test="$isInPrintf='yes'">
	<SgVarRefExp name="i"/>
      </xsl:if>
    </xsl:copy>
  </xsl:template>


<!-- main関数の最初にint i=2;を挿入
   (本当はmain関数のbodyだと確認してから変数宣言を挿入すべきだけど
    関数が一つしかないから手抜き)
-->
  <xsl:template match="SgFunctionDefinition/SgBasicBlock">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
  int i = 2;
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>


<!--printfフラグのデバッグ用ルール -->
<!--
  <xsl:template match="SgFunctionRefExp/@symbol">
    <xsl:param name="isInPrintf"/>
    <xsl:choose>
      <xsl:when test="$isInPrintf='yes'">
	<xsl:attribute name="symbol">
	  <xsl:value-of select="string('yes')"/>
	</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
	<xsl:attribute name="symbol">
	  <xsl:value-of select="string('no')"/>
	</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
-->
</xsl:stylesheet>
