<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:exslt="http://exslt.org/common" >

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="/">
		<xsl:apply-templates />
	</xsl:template>

	<xsl:template match="*">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates />
		</xsl:copy>
	</xsl:template>


	<!-- SgVarRefExp -->
	<xsl:template match="SgVarRefExp">
		<xsl:choose>
			
			<!--	!$xev scalar2array1-varref start(スカラ変数,添字)
			
				変数の参照をスカラ変数から1次元配列に置き換える
				2014.03.10
				2014.03.18
			-->
			<xsl:when test="./@name=//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1-varref']/CLAUSE[@name='start']/LI[1]/@value">
				<xsl:choose>
					<!-- 
						現在ノードより文書順で前にある最も近いノードが、
						範囲指定終了【!$xev end scalar2array1-varref(スカラ変数,サイズ,添字)】の場合
						変換処理を行わない
					<xsl:when test="preceding::DIRECTIVE[ @name='end' and ./CLAUSE/@name='scalar2array1-varref' and ./CLAUSE/LI[1]/@value=current()/@name][1]/CLAUSE/LI[1]/@value='current()/@name'">
					-->
					<xsl:when test="preceding::DIRECTIVE[./CLAUSE/LI[1]/@value=current()/@name and @name='end' or @name='scalar2array1-varref'][1]/CLAUSE/@name='scalar2array1-varref'">
						<xsl:copy>
							<xsl:copy-of select="@*"/>
							<xsl:apply-templates/>
						</xsl:copy>
					</xsl:when>

					<xsl:otherwise>
						<xsl:element name="SgPntrArrRefExp">
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">
									<xsl:value-of select="concat(./@name,'_tmp')"/>
								</xsl:attribute>
							</xsl:element>

							<xsl:element name="SgExprListExp">
								<xsl:element name="SgVarRefExp">
									<xsl:attribute name="name">
										<!--
										<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1']/CLAUSE[@name='varref']/LI[1][@value=current()/@name]/following-sibling::*[1]/@value"/>
										<xsl:value-of select="preceding::DIRECTIVE[ @name='scalar2array1' and ./CLAUSE/@name='varref' and ./CLAUSE/LI[1]/@value=current()/@name ][1]/CLAUSE/LI[2]/@value"/>
										-->
										<xsl:value-of select="preceding::DIRECTIVE[ @name='scalar2array1-varref' and ./CLAUSE/@name='start' and ./CLAUSE/LI[1]/@value=current()/@name ][1]/CLAUSE/LI[3]/@value"/>

									</xsl:attribute>
								</xsl:element>
							</xsl:element>
		

						</xsl:element>

						<xsl:apply-templates/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- SgExprListExp -->
	<xsl:template match="SgExprListExp">
		<xsl:choose>
			<!--	!$xev array1to2-varref start(変数名,添字,添字)
			
				変数の参照を1次元配列から2次元配列に置き換える
				2014.03.07
			-->

			<xsl:when test="preceding-sibling::SgVarRefExp[1]/@name=preceding::DIRECTIVE[@name='array1to2-varref']/CLAUSE[@name='start']/LI[1]/@value">
				<xsl:choose>
					<!-- 
						現在ノードより文書順で前にある最も近いノードが、
						範囲指定終了【!$xev end array1to2-varref(変数名,添字,添字)】の場合
						ディフォルト値を設定する
					-->
					<xsl:when test="preceding::DIRECTIVE[./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name and @name='end' or @name='array1to2-varref'][1]/CLAUSE/@name='array1to2-varref'">
						<xsl:choose>
							<!--
								ディフォルト値の指定【!$xev array1to2-varref start(変数名,添字,添字,default)】がある場合、ディフォルト値を設定する
							-->
							<xsl:when test="//DIRECTIVE[ @name='array1to2-varref' and ./CLAUSE/@name='start' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name]/CLAUSE/LI[4]/@value='default'">
								<xsl:copy>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2-varref' and ./CLAUSE/@name='start' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name and ./CLAUSE/LI[4]/@value='default']/CLAUSE/LI[2]/@value"/>

										</xsl:attribute>
									</xsl:element>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2-varref' and ./CLAUSE/@name='start' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name and ./CLAUSE/LI[4]/@value='default']/CLAUSE/LI[3]/@value"/>
										</xsl:attribute>
									</xsl:element>
								</xsl:copy>

							</xsl:when>

							<!--
								ディフォルト値の指定がない場合
								はじめに見つかった【!$xev array1to2-varref start(変数名,添字,添字)】の添字を設定する
							<xsl:otherwise>
								<xsl:copy>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2-varref' and ./CLAUSE/@name='start' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name]/CLAUSE/LI[2]/@value"/>

										</xsl:attribute>
									</xsl:element>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2-varref' and ./CLAUSE/@name='start' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name]/CLAUSE/LI[3]/@value"/>
										</xsl:attribute>
									</xsl:element>
								</xsl:copy>
							</xsl:otherwise>
							-->
						</xsl:choose>
					</xsl:when>

					<!-- 
						現在ノードより文書順で前にある【!$xev array1to2-varref start(変数名,添字,添字)】の
						添字設定する
					-->
					<xsl:otherwise>
						<xsl:copy>
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">
									<xsl:value-of select="preceding::DIRECTIVE[ @name='array1to2-varref' and ./CLAUSE/@name='start' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name][1]/CLAUSE/LI[2]/@value"/>

								</xsl:attribute>
							</xsl:element>
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">
									<xsl:value-of select="preceding::DIRECTIVE[ @name='array1to2-varref' and ./CLAUSE/@name='start' and ./CLAUSE/LI[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name][1]/CLAUSE/LI[3]/@value"/>
								</xsl:attribute>
							</xsl:element>

						</xsl:copy>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- SgInitializedName -->
	<xsl:template match="SgInitializedName">
		<xsl:choose>
			<!--	!$xev scalar2array1-varref start(スカラ変数,サイズ,添字)
			
				スカラ変数のワーク用１次元配列を追加する
					Ex.  INTEGER hoge　→　INTEGER hoge,hoge_tmp(サイズ)
				2014.03.12
			-->
			<xsl:when test="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1-varref']/CLAUSE[@name='start']/LI[1]/@value=./@name">
				<xsl:copy-of select="."/>
				<xsl:copy>
					<xsl:attribute name="name">
							<xsl:value-of select="concat(./@name,'_tmp')"/>
					</xsl:attribute>
					<xsl:element name="SgArrayType">
						<xsl:attribute name="index">""</xsl:attribute>
						<xsl:attribute name="rank">1</xsl:attribute>
						<xsl:attribute name="type">
							<xsl:value-of select="./SgArrayType/@type"/>
						</xsl:attribute>
					
						<xsl:copy-of select="./*[1]"/>

						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1-varref']/CLAUSE[@name='start']/LI[1][@value=current()/@name]/following-sibling::*[1]/@value"/>
							</xsl:attribute>
						</xsl:element>
					</xsl:element>
				</xsl:copy>
			</xsl:when>


			<!--	!$xev array1to2 type(変数名,サイズ,サイズ)
			
				1次元配列の宣言を2次元配列に置き換える
				2014.03.06
			<xsl:when test="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type']/LI[1]/@value=./@name">
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:element name="SgArrayType">
						<xsl:attribute name="index">
							<xsl:value-of select="./SgArrayType/@index"/>
						</xsl:attribute>
						<xsl:attribute name="rank">2</xsl:attribute>
						<xsl:attribute name="type">
							<xsl:value-of select="./SgArrayType/@type"/>
						</xsl:attribute>
					
						<xsl:copy-of select="./SgArrayType/*[1]"/>

						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type']/LI[1][@value=current()/@name]/following-sibling::*[1]/@value"/>
							</xsl:attribute>
						</xsl:element>
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type']/LI[1][@value=current()/@name]/following-sibling::*[2]/@value"/>
							</xsl:attribute>
						</xsl:element>
					</xsl:element>
				</xsl:copy>
			</xsl:when>
			-->


			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>




	<!-- SgVariableDeclaration -->
	<xsl:template match="SgVariableDeclaration">
		<xsl:choose>
			<!--	!$xev one-loop-copy flag( サイズ,サイズ, ... )

				子ノードに【!$xev one-loop-copy flag】が存在する場合、
				コピー有無フラグを追加宣言する
			-->
			<xsl:when test=".=//SgVariableDeclaration[last()]" >
				<xsl:choose>
			<xsl:when test="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']" >
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
				<xsl:copy>
					<SgInitializedName name="xev_flg">
						<SgArrayType>
							<xsl:attribute name="index">""</xsl:attribute>
							<xsl:attribute name="rank">
								<xsl:value-of select="count(//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']/LI)"/>
							</xsl:attribute>
							<xsl:attribute name="type">"SgTypeInt"</xsl:attribute>
							<SgTypeInt/>
					

							<xsl:for-each select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']/LI">
 								<SgVarRefExp>
									<xsl:attribute name="name">
										<xsl:value-of select="@value" />
									</xsl:attribute>
								</SgVarRefExp>
							</xsl:for-each>
						</SgArrayType>
					</SgInitializedName>

				</xsl:copy>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
				</xsl:choose>
			</xsl:when>


			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- SgExprStatement -->
	<xsl:template match="SgExprStatement">
		<xsl:choose>
			<!--	!$xev statement-del ptn-001

				直前に'statement-del'がある場合、この statement を削除する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='statement-del']/CLAUSE[@name='ptn-001']">
				<xsl:comment>
					remove statement
				</xsl:comment>
			</xsl:when>

			<!--	!$xev statement-rep ptn-001

				直前に'statement-rep'がある場合、この statement を置き換える
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='statement-rep']/CLAUSE[@name='ptn-001']">
				<xsl:comment>
					remove statement
				</xsl:comment>
                        	<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='statement-rep']/CLAUSE[@name='ptn-001']/LI[1]/@value" />
                        	<xsl:text>,</xsl:text>
                                <xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='statement-rep']/CLAUSE[@name='ptn-001']/LI[2]/@value" />
                                <xsl:text>)=B(i,j)</xsl:text>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	
	<!-- SgWhileStmt -->
	<xsl:template match="SgWhileStmt">
		<xsl:choose>
			<!--	!$xev while2do replace(変数,初期値,最終値[,刻み幅])

				直前に'while2do replace'がある場合、この【WHILE文】を【do文】に置き換える
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']">
				<xsl:element name="SgFortranDo">
					<!-- 【WHILE文】の属性をそのまま使用する -->
					<xsl:copy-of select="@*"/>

					<!-- 【変数=初期値】 -->
					<xsl:element name="SgAssignOp">
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/LI[1]/@value"/>
							</xsl:attribute>
						</xsl:element>
						<xsl:element name="SgIntVal">
							<xsl:attribute name="value">
								<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/LI[2]/@value"/>
							</xsl:attribute>
						</xsl:element>
					</xsl:element>

					<!-- 【最終値】 -->
					<xsl:element name="SgVarRefExp">
						<xsl:attribute name="name">
							<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/LI[3]/@value"/>
						</xsl:attribute>
					</xsl:element>

					<!-- 【刻み幅】 -->
					<xsl:choose>
						<!-- 【刻み幅】の指定があるとき -->
						<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/LI[4]/@value">
							<xsl:element name="SgIntVal">
								<xsl:attribute name="value">
									<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/LI[4]/@value"/>
								</xsl:attribute>
							</xsl:element>
						</xsl:when>

						<!-- 【刻み幅】の指定がないとき、ディフォルト値(1)を設定する -->
						<xsl:otherwise>
							<SgNullExpression/>
						</xsl:otherwise>
					</xsl:choose>

					<!-- 
					【WHILE文】の処理内容【SgBasicBlock】を変換しないで複写する
                        		<xsl:copy-of select="./SgBasicBlock" />
                        		-->
					<!-- 
					【WHILE文】の処理内容【SgBasicBlock】をテンプレートを使用して複写する
                        		-->
					<xsl:apply-templates select="./SgBasicBlock"/>

				</xsl:element>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- SgFortranDo -->
	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<!--	!$xev loop-unroll step(刻み幅)

				直前に'loop-unroll step'がある場合、直下のDO文の刻み幅を変更する
				実行行を刻み幅分実行する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop-unroll']/CLAUSE[@name='step']">
				<xsl:copy>
					<xsl:copy-of select="@*"/>

					<!-- 変数 初期値 -->
					<xsl:copy-of select="./*[1]"/>

					<!-- 最終値 -->
					<xsl:copy-of select="./*[2]"/>
					<!-- 刻み幅 -->
					<xsl:element name="SgIntVal">
						<xsl:attribute name="value">
							<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop-unroll']/CLAUSE[@name='step']/LI[1]/@value"/>
						</xsl:attribute>
					</xsl:element>
					
  					<!-- multi-pass
						変換した内容を(tmp)に格納し
						その(tmp)内容を再変換する
					-->
					<!-- 変換内容を、tmp に格納する -->
  					<xsl:variable name="tmp">
						<xsl:apply-templates select="./SgBasicBlock" mode="loop-unroll">
							<xsl:with-param name="max" select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop-unroll']/CLAUSE[@name='step']/LI[1]/@value"/>
							<xsl:with-param name="var" select="./SgAssignOp/SgVarRefExp/@name"/>
						</xsl:apply-templates>
			  		</xsl:variable>
					<!-- 変換内容(tmp)を、再変換して出力する -->
			  		<xsl:apply-templates select="exslt:node-set($tmp)"/>

				</xsl:copy>

			</xsl:when>

			<!--	!pragma xev one-loop-copy flag(n,n,n)

				直前に'one-loop-copy flag'がある場合、直下の多重ループコピーを
				１重ループのコピーに変換する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']">

				<!--
					マスクフラグを初期化する
				-->
				<xsl:param name="idx1" select="0"/>
				<xsl:apply-templates select="." mode="one-loop-copy-0">
					<xsl:with-param name="idx1" select="$idx1 + 1"/>
				</xsl:apply-templates>


				<!--
					マスクフラグを設定する
				-->
				<xsl:apply-templates select="." mode="one-loop-copy-1"/>

				<!--
					マスクフラグを参照しデータをコピーする
				-->
				<xsl:param name="idx2" select="0"/>
				<xsl:apply-templates select="." mode="one-loop-copy-c">
					<xsl:with-param name="idx2" select="$idx2 + 1"/>
				</xsl:apply-templates>

			</xsl:when>

			<!--	!$xev fortran-do copy-001(a(i,j) = b(i,j))

				直前に'fortran-do copy-001'がある場合、この【DO文】をコピーし、
				処理部に【a(i,j) = b(i,j)】を設定する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='fortran-do']/CLAUSE[@name='copy-001']">
				<!-- 既存の【SgFortranDo】をそのままコピーする -->
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>

				<!--
					【SgFortranDo】の処理内容を【a(i,j) = b(i,j)】に変更する
					mode="fortran-do-copy-001"のテンプレートを使用してコピーする
				-->
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates select="./*" mode="fortran-do-copy-001"/>
				</xsl:copy>

			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!--
		最後の【SgExprStatement】行の場合
	-->
	<xsl:template match="SgExprStatement[last()]" mode="loop-unroll">
		<xsl:param name="max"/>		<!-- STEP数 -->
		<xsl:param name="var"/>		<!-- 置き換える変数 -->

		<!-- 自分を出力する -->
		<xsl:apply-templates select="."/>

		<!-- コピーする【SgExprStatement】行を設定 -->
		<xsl:param name="copy-stm" select="../SgExprStatement"/>

		<!-- コピーする【SgExprStatement】行を設定 -->
		<xsl:for-each select="(//*)[position()&lt;$max]">
			<xsl:apply-templates select="$copy-stm" mode="loop-unroll-1">
				<xsl:with-param name="max" select="$max"/>
				<xsl:with-param name="var" select="$var"/>
				<xsl:with-param name="cnt" select="position()"/>
			</xsl:apply-templates>
  		</xsl:for-each>

	</xsl:template>
	<xsl:template match="*" mode="loop-unroll">
		<xsl:param name="max"/>
		<xsl:param name="var"/>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="loop-unroll">
				<xsl:with-param name="max" select="$max"/>
				<xsl:with-param name="var" select="$var"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo"  mode="loop-unroll">
	</xsl:template>
	<!--
	<xsl:template match="SgPragmaDeclaration[1]" mode="loop-unroll">
	</xsl:template>
	-->


	<xsl:template match="SgVarRefExp" mode="loop-unroll-1">
		<xsl:param name="max"/>
		<xsl:param name="var"/>
		<xsl:param name="cnt"/>

		<xsl:if test="./@name=$var">
			<SgAddOp>
				<xsl:copy-of select="." />
				<SgIntVal>
					<xsl:attribute name="value">
						<xsl:value-of select="$cnt"/>
					</xsl:attribute>
				</SgIntVal>
			</SgAddOp>
		</xsl:if>

		<xsl:if test="./@name!=$var">
			<xsl:copy-of select="." />
		</xsl:if>
	</xsl:template>

	<xsl:template match="*" mode="loop-unroll-1">
		<xsl:param name="max"/>
		<xsl:param name="var"/>
		<xsl:param name="cnt"/>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="loop-unroll-1">
				<xsl:with-param name="max" select="$max"/>
				<xsl:with-param name="var" select="$var"/>
				<xsl:with-param name="cnt" select="$cnt"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>



	<!-- 
		マスクフラグを初期化するテンプレート【ここから】 
		DO文をコピーして、マスクフラグ領域に０を設定するテンプレート
	-->
	<xsl:template match="SgFortranDo" mode="one-loop-copy-0">
		<xsl:param name="idx1" select="1"/>
		<xsl:copy>
			<!-- 属性をそのまま使用する -->
			<xsl:copy-of select="@*"/>

			<!-- 変数 -->
			<SgAssignOp>
				<SgVarRefExp>
					<xsl:attribute name="name">
						<xsl:value-of select="./SgAssignOp/SgVarRefExp/@name"/>
					</xsl:attribute>
				</SgVarRefExp>
				<!-- 初期値 -->
				<SgIntVal value="1"/>
			</SgAssignOp>

			<!-- 最終値 -->
			<SgVarRefExp>
				<xsl:attribute name="name">
					<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']/LI[$idx1]/@value"/>
				</xsl:attribute>
			</SgVarRefExp>
			<!-- 刻み幅 -->
			<SgNullExpression/>
			<xsl:apply-templates mode="one-loop-copy-0">
				<xsl:with-param name="idx1" select="$idx1 + 1"/>
			</xsl:apply-templates>
			<!--
			<xsl:apply-templates select="./SgBasicBlock" mode="one-loop-copy-00"/>
			-->
		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgExprStatement" mode="one-loop-copy-0">
		<xsl:copy>
			<!-- マスクフラグに０を設定する -->
			<SgAssignOp>
				<SgPntrArrRefExp>
					<SgVarRefExp name="xev_flg"/>
					<SgExprListExp>
						<xsl:for-each select="./SgAssignOp/SgPntrArrRefExp[1]/SgExprListExp/SgVarRefExp">
 							<SgVarRefExp>
								<xsl:attribute name="name">
									<xsl:value-of select="@name" />
								</xsl:attribute>
							</SgVarRefExp>
						</xsl:for-each>
					</SgExprListExp>
				</SgPntrArrRefExp>
				<SgIntVal value="0"/>
			</SgAssignOp>
		</xsl:copy>
	</xsl:template>
	<xsl:template match="*" mode="one-loop-copy-0">
		<xsl:param name="idx1" select="1"/>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="one-loop-copy-0">
				<xsl:with-param name="idx1" select="$idx1"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo"  mode="one-loop-copy-0">
	</xsl:template>
	<!-- remove SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration" mode="one-loop-copy-0">
	</xsl:template>


	<!-- 
		マスクフラグにコピーする領域を設定するテンプレート【ここから】 
		DO文をコピーして、マスクフラグ領域に１を設定する
	-->
	<xsl:template match="SgExprStatement" mode="one-loop-copy-1">
		<xsl:copy>
			<!-- マスクフラグに１を設定する -->
			<SgAssignOp>
				<SgPntrArrRefExp>
					<SgVarRefExp name="xev_flg"/>
					<SgExprListExp>
						<xsl:for-each select="./SgAssignOp/SgPntrArrRefExp[1]/SgExprListExp/SgVarRefExp">
 							<SgVarRefExp>
								<xsl:attribute name="name">
									<xsl:value-of select="@name" />
								</xsl:attribute>
							</SgVarRefExp>
						</xsl:for-each>
					</SgExprListExp>
				</SgPntrArrRefExp>
				<SgIntVal value="1"/>
			</SgAssignOp>
		</xsl:copy>
	</xsl:template>
	<xsl:template match="*" mode="one-loop-copy-1">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="one-loop-copy-1"/>
		</xsl:copy>
	</xsl:template>
	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo"  mode="one-loop-copy-1">
	</xsl:template>
	<!-- remove SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration" mode="one-loop-copy-1">
	</xsl:template>
	<!-- SgFortranDoをコピーして、コピーする領域に１を設定する【ここから】 -->


	<!-- 
		マスクフラグを参照してデータをコピーするテンプレート【ここから】 
		DO文をコピーして、マスクフラグ領域に０を設定するテンプレート
	-->
	<xsl:template match="SgFortranDo" mode="one-loop-copy-c">
		<!-- DO文の階層を変数に設定する -->
		<xsl:param name="idx2" select="1"/>
		<xsl:choose>
			<xsl:when test="$idx2=count(//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']/LI)">
				<xsl:copy>
					<!-- 属性をそのまま使用する -->
					<xsl:copy-of select="@*"/>

					<!-- 変数 -->
					<SgAssignOp>
						<SgVarRefExp>
							<xsl:attribute name="name">
								<xsl:value-of select="./SgAssignOp/SgVarRefExp/@name"/>
							</xsl:attribute>
						</SgVarRefExp>
						<!-- 初期値 -->
						<SgIntVal value="1"/>
					</SgAssignOp>

					<!-- 最終値 (n*n*n)-->
					<xsl:call-template name="loop_size">
						<xsl:with-param name="lop" select="count(//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']/LI)"/>
					</xsl:call-template>

					<!-- 刻み幅 -->
					<SgNullExpression/>
					<xsl:apply-templates select="./SgBasicBlock" mode="one-loop-copy-c">
						<xsl:with-param name="idx2" select="$idx2+1"/>
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:when>

			<xsl:otherwise>
				<xsl:apply-templates select="./SgBasicBlock" mode="one-loop-copy-c">
					<xsl:with-param name="idx2" select="$idx2+1"/>
				</xsl:apply-templates>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="loop_size">
		<xsl:param name="lop" select="3"/>
		<SgMultiplyOp>
			<xsl:if test="$lop!=2">
				<xsl:call-template name="loop_size">
					<xsl:with-param name="lop" select="$lop - 1"/>
				</xsl:call-template>
			</xsl:if>
			<xsl:if test="$lop=2">
				<SgVarRefExp>
					<xsl:attribute name="name">
						<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']/LI[1]/@value" />
					</xsl:attribute>
				</SgVarRefExp>
			</xsl:if>

			<SgVarRefExp>
				<xsl:attribute name="name">
					<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']/LI[$lop]/@value" />
				</xsl:attribute>
			</SgVarRefExp>
		</SgMultiplyOp>
	</xsl:template>



	<xsl:template match="SgExprStatement" mode="one-loop-copy-c">

		<SgIfStmt>
			<SgExprStatement>
				<SgEqualityOp>
					<SgPntrArrRefExp>
						<SgVarRefExp name="xev_ifg"/>
						<!--
						<SgExprListExp>
							<SgVarRefExp>
								<xsl:attribute name="name">
									<xsl:value-of select="./SgAssignOp/SgPntrArrRefExp[1]/SgExprListExp/SgVarRefExp[1]/@name" />
								</xsl:attribute>
							</SgVarRefExp>
							<xsl:variable name="count" select="count(//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']/LI)-1"/>
							<xsl:for-each select="(//*)[position()&lt;=$count]">
								<SgIntVal value="1"/>
  							</xsl:for-each>
						</SgExprListExp>
						-->
						<xsl:apply-templates select="./SgAssignOp/SgPntrArrRefExp/SgExprListExp" mode="one-loop-copy-c"/>
					</SgPntrArrRefExp>
					<SgIntVal value="1"/>
				</SgEqualityOp>
			</SgExprStatement>
			<SgBasicBlock>

				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="one-loop-copy-c"/>
				</xsl:copy>
			</SgBasicBlock>
		</SgIfStmt>
	</xsl:template>

	<xsl:template match="SgExprListExp" mode="one-loop-copy-c">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<SgVarRefExp>
				<xsl:attribute name="name">
					<xsl:value-of select="./SgVarRefExp[1]/@name" />
				</xsl:attribute>
			</SgVarRefExp>
			<xsl:variable name="count" select="count(//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']/LI)-1"/>
			<xsl:for-each select="(//*)[position()&lt;=$count]">
				<SgIntVal value="1"/>
  			</xsl:for-each>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="*" mode="one-loop-copy-c">
		<xsl:param name="idx2" select="1"/>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="one-loop-copy-c">
				<xsl:with-param name="idx2" select="$idx2"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo"  mode="one-loop-copy-c">
	</xsl:template>
	<!-- remove SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration" mode="one-loop-copy-c">
	</xsl:template>



	<!-- SgFortranDoをコピーして、処理に【a(i,j) = b(i,j)】を設定する【ここから】 -->
	<!-- SgExprStatement -->
	<xsl:template match="SgExprStatement" mode="fortran-do-copy-001">
		<xsl:copy>
			<SgAssignOp>
				<SgPntrArrRefExp>
					<SgVarRefExp name="a"/>
					<SgExprListExp>
						<SgVarRefExp name="i"/>
						<SgVarRefExp name="j"/>
					</SgExprListExp>
				</SgPntrArrRefExp>
				<SgPntrArrRefExp>
					<SgVarRefExp name="b"/>
					<SgExprListExp>
						<SgVarRefExp name="i"/>
						<SgVarRefExp name="j"/>
					</SgExprListExp>
				</SgPntrArrRefExp>
			</SgAssignOp>
		</xsl:copy>
	</xsl:template>
	<xsl:template match="*" mode="fortran-do-copy-001">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="fortran-do-copy-001"/>
		</xsl:copy>
	</xsl:template>
	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo"  mode="fortran-do-copy-001">
	</xsl:template>
	<!-- remove SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration" mode="fortran-do-copy-001">
	</xsl:template>



	<!-- SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration">
		<xsl:choose>
			<!--	!$xev loop-unroll step(刻み幅)

				ディレクティブを削除する
			<xsl:when test="./SgPragma/DIRECTIVE[@name='loop-unroll'and ./CLAUSE/@name='step'][1]">
			<xsl:when test="./SgPragma/DIRECTIVE[@name='loop-unroll']/CLAUSE[@name='step'] and not(contains(./SgPragma/DIRECTIVE[@name='loop-unroll']/CLAUSE[@name='step']/LI[1]/@value,'default'))">
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='loop-unroll']/CLAUSE[@name='step'] and not(contains(./SgPragma/DIRECTIVE[@name='loop-unroll']/CLAUSE[@name='step']/LI[1]/@value,'default'))">
				<xsl:comment>
					SgPragmaDeclaration loop-unroll
				</xsl:comment>
			</xsl:when>

			<!--	!$xev one-loop-copy flag(n,n,n)

				ディレクティブを削除する
			-->

			<xsl:when test="./SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag'] and not(contains(./SgPragma/DIRECTIVE[@name='one-loop-copy']/CLAUSE[@name='flag']/LI[1]/@value,'default'))">
				<xsl:comment>
					SgPragmaDeclaration one-loop-copy
				</xsl:comment>
			</xsl:when>


			<!--	!$xev scalar2array1-varref start(変数名,サイズ,添字)

				追加した１次元配列を初期化する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='scalar2array1-varref']/CLAUSE[@name='start'] and not(contains(./SgPragma/DIRECTIVE[@name='scalar2array1-varref']/CLAUSE[@name='start']/LI[1]/@value,'default'))">
          
				<SgFortranDo end="1" nlabel="" slabel="" style="0">
					<SgAssignOp>
						<SgVarRefExp>
							<xsl:attribute name="name">
								<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1-varref']/CLAUSE[@name='start']/LI[3]/@value"/>
							</xsl:attribute>
						</SgVarRefExp>
						<SgIntVal value="1"/>
					</SgAssignOp>
					<SgVarRefExp>
						<xsl:attribute name="name">
							<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1-varref']/CLAUSE[@name='start']/LI[2]/@value"/>
						</xsl:attribute>
					</SgVarRefExp>
					<SgNullExpression/>
					<SgBasicBlock>
						<SgExprStatement>
							<SgAssignOp>
								<SgPntrArrRefExp>
									<SgVarRefExp>
										<xsl:attribute name="name">
											<xsl:value-of select="concat(./SgPragma/DIRECTIVE[@name='scalar2array1-varref']/CLAUSE[@name='start']/LI[1]/@value,'_tmp')"/>
										</xsl:attribute>
									</SgVarRefExp>
									<SgExprListExp>
										<SgVarRefExp>
											<xsl:attribute name="name">
												<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1-varref']/CLAUSE[@name='start']/LI[3]/@value"/>
											</xsl:attribute>
										</SgVarRefExp>
									</SgExprListExp>
								</SgPntrArrRefExp>
								<SgIntVal value="0"/>
							</SgAssignOp>
						</SgExprStatement>
					</SgBasicBlock>
				</SgFortranDo>
			</xsl:when>

			<!--	!$xev array1to2 varref(変数名,添字,添字)

				１次元配列を２次元配列に置き替える
			-->

			<xsl:when test="./SgPragma/DIRECTIVE[@name='array1to2-varref']/CLAUSE[@name='start'] and not(contains(./SgPragma/DIRECTIVE[@name='array1to2-varref']/CLAUSE[@name='start']/LI[1]/@value,'default'))">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev array1to2 type(変数名,サイズ,サイズ)

				１次元配列を２次元配列に置き替える
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type'] and not(contains(./SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type']/LI[1]/@value,'default'))">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev dir ,add

				ディレクティブを追加する	
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='add'] and not(contains(./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='add']/LI[1]/@value,'default'))">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='add']/LI">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>

			<!--	!$xev dir replace

				ディレクティブを置換する	
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace'] and not(contains(./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace']/LI[1]/@value,'default'))">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace']/LI">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>

			<!--	!$xev dir del(delete)

                                ディレクティブを削除する
                        -->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='del'] and not(contains(./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='del']/LI[1]/@value,'default'))">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>


			<!--	!$xev dir append( [文字列] )

                                ディレクティブ行に[文字列]を追加する

				2014.03.06
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append'] and not(contains(./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append']/LI[1]/@value,'default'))">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<!--
								追加するディレクティブ行の内容を取り出す

								指示内容（!$xev dir append( 文字列 )と同じ【PreprocessingInfo]】ノードの
								直下の【PreprocessingInfo]】を取り出す
							-->
							<xsl:value-of select="substring( substring-after(following-sibling::*/PreprocessingInfo[contains(text(),current()/SgPragma/@pragma)]/following-sibling::*[1],'!$'), 1, string-length(substring-after(following-sibling::*/PreprocessingInfo[contains(text(),current()/SgPragma/@pragma)]/following-sibling::*[1],'!$'))-1)"/>

							<!--
								接続時の半角スペースを挿入
							-->
							<xsl:text> </xsl:text>

							<!-- 
								追加する文字列を取り出
							-->
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append']/LI">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>

			<!--	!$xev dir append()
                                ディレクティブを削除する
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>
			-->


			<!--	!$xev statement-add ptn-000( )

				指定文字列をXMLに出力する
			-->
                        <xsl:when test="./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-000'] and not(contains(./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-000']/LI[1]/@value,'default'))">
				<xsl:for-each select="./SgPragma/DIRECTIVE[@name='statement-add']/CLAUSE[@name='ptn-000']/LI">
					<xsl:value-of select="@value" /><xsl:text> </xsl:text>
				</xsl:for-each>
			</xsl:when>



			<!--	!$xev statement-del ptn-001
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='statement-del']/CLAUSE[@name='ptn-001']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev statement-rep ptn-001
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='statement-rep']/CLAUSE[@name='ptn-001']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev var-type-chg ptn-001
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='var-type-chg']/CLAUSE[@name='ptn-001']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$while2do ptn-001
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev end array1to2-varref(変数名,添字,添字)

				１次元配列を２次元配列に置き替える
			-->

			<xsl:when test="./SgPragma/DIRECTIVE/@name='end'">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>



			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo">
		<xsl:choose>
			<!--直前に'!$xev dir append'がある場合、この行を削除する -->
			<xsl:when test="contains(preceding-sibling::PreprocessingInfo[1],'!$xev dir append')">
				<xsl:comment>
					PreprocessingInfo
				</xsl:comment>
			</xsl:when>

			<!--	!$xev dir append( [文字列] )

				直前に'!$xev dir append'がある場合
                                ディレクティブ行に[文字列]を追加する
			<xsl:when test="contains(preceding-sibling::PreprocessingInfo[1],'!$xev dir append')">
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:value-of select="concat( substring(.,1,string-length(.)-1), ' ', substring( substring-after(preceding-sibling::PreprocessingInfo[1],'('), 1, string-length(substring-after(preceding-sibling::PreprocessingInfo[1],'('))-2) )"/>
				</xsl:copy>
			</xsl:when>
			-->

			<!--'!$xev'で始まる行を削除する -->
			<xsl:when test="contains(.,'!$xev')" >
				<xsl:comment>
					PreprocessingInfo
				</xsl:comment>
			</xsl:when>

			<!--直前に'!$xev dir replace'がある場合、この行を削除する -->
			<xsl:when test="contains(preceding-sibling::PreprocessingInfo[1],'!$xev dir replace')">
				<xsl:comment>
					PreprocessingInfo
				</xsl:comment>
			</xsl:when>

			<!--直前に'!$xev dir del'がある場合、この行を削除する -->
			<xsl:when test="contains(preceding-sibling::PreprocessingInfo[1],'!$xev dir del')">
				<xsl:comment>
					PreprocessingInfo
				</xsl:comment>
			</xsl:when>

			<!-- コメント行は、そのままコピーする -->
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


</xsl:stylesheet>
