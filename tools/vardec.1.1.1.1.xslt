<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="SgExprStatement[SgFunctionCallExp/SgFunctionRefExp[@name='xevparse_dummy']]"/>

  <xsl:template match="SgProcedureHeaderStatement[@name='xevparse_dummy']"/>

  <xsl:template match="xev_pragma[xev_target/SgGlobal]">
    <xsl:element name="SgGlobal">
      <xsl:element name="xev_pragma">
	<xsl:apply-templates select="*[1]"/>
	<xsl:element name="xev_target">
	  <xsl:apply-templates select="xev_target/SgGlobal/*"/>
	</xsl:element>
	<xsl:apply-templates select="*[3]"/>
      </xsl:element>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgIfStmt[count(*[3]/*)=0]">
    <xsl:element name="SgIfStmtWithoutElse">
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="*"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgGlobal">
    <xsl:apply-templates select="preceding-sibling::SgPragmaDeclaration"/>
    <xsl:copy>
      <xsl:apply-templates select="*"/>
    </xsl:copy>
  </xsl:template>
  
  <xsl:template match="SgVarRefExp[@lvalue][@type]">
    <xsl:copy>
      <xsl:copy-of select="@name"/>
      <xsl:copy-of select="@type"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgVarRefExp[count(@type)=0]">
    <xsl:copy>
      <xsl:copy-of select="@name"/>
      <xsl:attribute name="type">_</xsl:attribute>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgFunctionParameterList">
    <xsl:element name="SgFunctionParameterList">
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates mode="funcparam" select="*"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="*[count(self::SgInitializedName)=0]" mode="funcparam"/>

  <xsl:template match="SgInitializedName" mode="funcparam">
    <xsl:element name="SgVarRefExp">
      <xsl:copy-of select="@name"/>
      <xsl:attribute name="type">_</xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="PreprocessingInfo" mode="funcparam">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgInitializedName[SgAssignInitializer/SgAggregateInitializer][parent::SgVariableDeclaration]">
    <xsl:if test="count(preceding-sibling::*) - count(preceding-sibling::xev_pramga) = 0">
      <xsl:apply-templates select="*[2]/*[1]"/>
    </xsl:if>
    <xsl:element name="SgAssignOp">
      <xsl:element name="SgPntrArrRefExp">
	<xsl:element name="SgVarRefExp">
	  <xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
	  <xsl:attribute name="type">_</xsl:attribute>
	</xsl:element>
	<xsl:apply-templates select="SgArrayType/SgExprListExp"/>
      </xsl:element>
      <xsl:apply-templates select="SgAssignInitializer/SgAggregateInitializer/*[1]"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgInitializedName[SgAssignInitializer][count(SgAssignInitializer/SgAggregateInitializer)=0][parent::SgVariableDeclaration]">
    <xsl:if test="count(preceding-sibling::*) - count(preceding-sibling::xev_pragma) = 0">
      <xsl:apply-templates select="*[2]"/>
    </xsl:if>
    <xsl:element name="SgAssignOp">
      <xsl:element name="SgVarRefExp">
	<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
	<xsl:attribute name="type">_</xsl:attribute>
      </xsl:element>
      <xsl:apply-templates select="SgAssignInitializer/*"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgInitializedName[SgConstructorInitializer][parent::SgVariableDeclaration]">
    <xsl:if test="count(preceding-sibling::*) - count(preceding-sibling::xev_pragma) = 0">
      <xsl:apply-templates select="*[2]"/>
    </xsl:if>
    <xsl:element name="SgAssignOp">
      <xsl:element name="SgVarRefExp">
	<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
	<xsl:attribute name="type">_</xsl:attribute>
      </xsl:element>
      <xsl:apply-templates select="SgConstructorInitializer"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgInitializedName[SgArrayType][count(SgAssignInitializer)=0][parent::SgVariableDeclaration]">
    <xsl:if test="count(preceding-sibling::*) - count(preceding-sibling::xev_pragma) = 0">
      <xsl:apply-templates select="SgArrayType/*[1]"/>
    </xsl:if>
    <xsl:element name="SgPntrArrRefExp">
      <xsl:element name="SgVarRefExp">
	<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
	<xsl:attribute name="type">_</xsl:attribute>
      </xsl:element>
      <xsl:apply-templates select="SgArrayType/SgExprListExp"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgInitializedName[SgPointerType/SgArrayType][parent::SgVariableDeclaration]">
    <xsl:if test="count(preceding-sibling::*) - count(preceding-sibling::xev_pragma) = 0">
      <xsl:element name="SgPointerType">
	<xsl:apply-templates select="SgPointerType/SgArrayType/*[1]"/>
      </xsl:element>
    </xsl:if>
    <xsl:element name="SgPntrArrRefExp">
      <xsl:element name="SgVarRefExp">
	<xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
	<xsl:attribute name="type">_</xsl:attribute>
      </xsl:element>
      <xsl:apply-templates select="SgPointerType/SgArrayType/SgExprListExp"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgInitializedName[count(SgArrayType)=0 and count(SgPointerType/SgArrayType)=0][count(SgAssignInitializer)=0 and count(SgConstructorInitializer)=0][parent::SgVariableDeclaration]">
    <xsl:if test="count(preceding-sibling::*) - count(preceding-sibling::xev_pragma) = 0">
      <xsl:apply-templates select="*[1]"/>
    </xsl:if>
    <xsl:element name="SgVarRefExp">
      <xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
      <xsl:attribute name="type">_</xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="SgInitializedName[parent::SgProcedureHeaderStatement]">
    <xsl:copy/>
  </xsl:template>

  <xsl:template match="SgAddOp|SgAndOp|SgConcatenationOp|SgDivideOp|SgEqualityOp|SgExponentiationOp|SgGreaterOrEqualOp|SgGreaterThanOp|SgLessOrEqualOp|SgLessOrEqualOp|SgLessThanOp|SgMultiplyOp|SgNotEqualOp|SgOrOp|SgPointerAssignOp|SgSubtractOp|SgUserDefinedBinaryOp|SgMinusOp|SgNotOp|SgUnaryAddOp|SgUserDefinedUnaryOp">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:if test="count(@paren)=0">
	<xsl:attribute name="paren">0</xsl:attribute>
      </xsl:if>
      <xsl:apply-templates select="*"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgVariableDeclaration">
    <xsl:apply-templates select="xev_pragma"/>
    <xsl:copy>
      <xsl:copy-of select="@declaration_modifier"/>
      <xsl:element name="modifier_list">
	<xsl:if test="(number(@type_modifier) mod 16) - (number(@type_modifier) mod 8) =  8">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">allocatable</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 32) - (number(@type_modifier) mod 16) =  16">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">asynchronous</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 64) - (number(@type_modifier) mod 32) =  32">
	  <xsl:element name="SgFunctionCallExp">
	    <xsl:element name="SgFunctionRefExp">
	      <xsl:attribute name="name">bind</xsl:attribute>
	    </xsl:element>
	    <xsl:element name="SgExprListExp">
	      <xsl:element name="SgVarRefExp">
		<xsl:attribute name="name"><xsl:value-of select="@link"/></xsl:attribute>
		<xsl:attribute name="type">_</xsl:attribute>
	      </xsl:element>
	      <xsl:if test="@bind">
		<xsl:element name="SgActualArgumentExpression">
		  <xsl:attribute name="name">name</xsl:attribute>
		  <xsl:element name="SgStringVal">
		    <xsl:attribute name="value"><xsl:value-of select="@bind"/></xsl:attribute>
		  </xsl:element>
		</xsl:element>
	      </xsl:if>
	    </xsl:element>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 128) - (number(@type_modifier) mod 64) =  64">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">data</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 512) - (number(@type_modifier) mod 256) =  256">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">intent(in)</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 1024) - (number(@type_modifier) mod 512) =  512">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">intent(out)</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 2048) - (number(@type_modifier) mod 1024) =  1024">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">intent(inout)</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 4096) - (number(@type_modifier) mod 2048) =  2048">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">intrinsic</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 8192) - (number(@type_modifier) mod 4096) =  4096">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">optional</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 16384) - (number(@type_modifier) mod 8192) =  8192">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">type_modifier8192</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 32768) - (number(@type_modifier) mod 16384) =  16384">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">type_modifier16384</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 65536) - (number(@type_modifier) mod 32768) =  32768">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">save</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 131072) - (number(@type_modifier) mod 65536) =  65536">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">target</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="(number(@type_modifier) mod 262144) - (number(@type_modifier) mod 131072) =  131072">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">value</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@access_modifier = '1'">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">private</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@access_modifier = '3'">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">public</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@cv_modifier = '2'">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">parameter</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@cv_modifier = '3'">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">volatile</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@storage_modifier = '2'">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">external</xsl:attribute>
	    <xsl:attribute name="single">1</xsl:attribute>
	  </xsl:element>
	</xsl:if>
      </xsl:element>
      <xsl:apply-templates select="*[count(xev_pragma)=0]"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgAttributeSpecificationStatement[@kind='1' or @kind='2' or @kind=3 or @kind=7 or @kind=8 or @kind=9 or @kind=10 or @kind=11 or @kind=12 or @kind=13 or @kind=15 or @kind=16 or @kind=17 or @kind=18]">
    <xsl:copy>
      <xsl:element name="modifier_list">
	<xsl:if test="@kind=1">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">private</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=2">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">public</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=9 and @intent=600">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">intent(in)</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=9 and @intent=601">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">intent(out)</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=9 and @intent=602">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">intent(inout)</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=10">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">intrinsic</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=11">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">optional</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=13">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">pointer</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=15">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">save</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=16">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">target</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=17">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">value</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=18">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">volatile</xsl:attribute>
	  </xsl:element>
	</xsl:if>
      </xsl:element>
      <xsl:apply-templates select="*" mode="attr"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgStringVal" mode="attr">
    <xsl:element name="SgVarRefExp">
      <xsl:attribute name="name"><xsl:value-of select="@value"/></xsl:attribute>
      <xsl:attribute name="type">_</xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="node()|@*" mode="attr">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgAttributeSpecificationStatement[@kind=3 or @kind=7 or @kind=12]">
    <xsl:copy>
      <xsl:element name="modifier_list">
	<xsl:if test="@kind=3">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">allocatable</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=7">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">dimension</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@kind=12">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">parameter</xsl:attribute>
	  </xsl:element>
	</xsl:if>
      </xsl:element>
      <xsl:apply-templates select="*"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgAttributeSpecificationStatement[@kind=8]">
    <xsl:copy>
      <xsl:element name="modifier_list">
	<xsl:if test="@kind=8">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">external</xsl:attribute>
	  </xsl:element>
	</xsl:if>
      </xsl:element>
      <xsl:apply-templates select="*" mode="external"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgFunctionRefExp" mode="external">
    <xsl:element name="SgVarRefExp">
      <xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
      <xsl:attribute name="type">_</xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="node()|@*" mode="external">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgProcedureHeaderStatement">
    <xsl:copy>
      <xsl:copy-of select="@kind"/>
      <xsl:copy-of select="@result"/>
      <xsl:copy-of select="@name"/>
      <xsl:copy-of select="@end_name"/>
      <xsl:element name="modifier_list">
	<xsl:if test="@pure=1">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">pure</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@recursive=1">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">recursive</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@elemental=1">
	  <xsl:element name="SgStringVal">
	    <xsl:attribute name="value">elemental</xsl:attribute>
	  </xsl:element>
	</xsl:if>
	<xsl:if test="@declaration_modifier=66">
	  <xsl:element name="SgFunctionCallExp">
	    <xsl:element name="SgFunctionRefExp">
	      <xsl:attribute name="name">bind</xsl:attribute>
	    </xsl:element>
	    <xsl:element name="SgExprListExp">
	      <xsl:element name="SgVarRefExp">
		<xsl:attribute name="name"><xsl:value-of select="@link"/></xsl:attribute>
		<xsl:attribute name="type">_</xsl:attribute>
	      </xsl:element>
	      <xsl:if test="@bind">
		<xsl:element name="SgActualArgumentExpression">
		  <xsl:attribute name="name">name</xsl:attribute>
		  <xsl:element name="SgStringVal">
		    <xsl:attribute name="value"><xsl:value-of select="@bind"/></xsl:attribute>
		  </xsl:element>
		</xsl:element>
	      </xsl:if>
	    </xsl:element>
	  </xsl:element>
	</xsl:if>
      </xsl:element>
      <xsl:apply-templates select="*"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgDataStatementGroup">
    <xsl:copy>
      <xsl:apply-templates select="SgDataStatementObject"/>
      <xsl:element name="SgDataStatementValue">
	<xsl:if test="SgDataStatementValue/@fmt=2">
	  <xsl:apply-templates select="SgDataStatementValue/SgExprListExp/*"/>
	</xsl:if>
	<xsl:if test="SgDataStatementValue/@fmt=3">
	  <xsl:element name="SgMultiplyOp">
	    <xsl:apply-templates select="SgDataStatementValue/*[2]"/>
	    <xsl:apply-templates select="SgDataStatementValue/*[3]"/>
	  </xsl:element>
	</xsl:if>
      </xsl:element>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgRenamePair">
    <xsl:if test="@lname = @uname">
      <xsl:element name="SgVarRefExp">
	<xsl:attribute name="name"><xsl:value-of select="@lname"/></xsl:attribute>
	<xsl:attribute name="type">_</xsl:attribute>
      </xsl:element>
    </xsl:if>
    <xsl:if test="@lname != @uname">
      <xsl:element name="SgPointerAssignOp">
	<xsl:attribute name="paren">0</xsl:attribute>
	<xsl:element name="SgVarRefExp">
	  <xsl:attribute name="name"><xsl:value-of select="@lname"/></xsl:attribute>
	  <xsl:attribute name="type">_</xsl:attribute>
	</xsl:element>
	<xsl:element name="SgVarRefExp">
	  <xsl:attribute name="name"><xsl:value-of select="@uname"/></xsl:attribute>
	  <xsl:attribute name="type">_</xsl:attribute>
	</xsl:element>
      </xsl:element>
    </xsl:if>
  </xsl:template>

  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
