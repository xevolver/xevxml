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

			<!--	!$xev var_replace var(変換前変数名,変換後変数名)
			-->
			<xsl:when test="./@name=//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='var_replace']/CLAUSE[@name='var' and @specified='true']/ARG[1]/@value">
				<xsl:choose>
					<!-- 
						現在ノードより文書順で前にある最も近いノードが、
						範囲指定終了【!$xev end var_replace var(変換前変数名,変換後変数名)】の場合
						変換処理を行わない【範囲外】
					-->
					<xsl:when test="preceding::DIRECTIVE[./CLAUSE/ARG[1]/@value=current()/@name and @name='end' or @name='var_replace'][1]/CLAUSE/@name='var_replace'">
						<xsl:copy>
							<xsl:copy-of select="@*"/>
							<xsl:apply-templates/>
						</xsl:copy>
					</xsl:when>
					<xsl:otherwise>
						<xsl:copy>
							<xsl:attribute name="name">
								<xsl:value-of select="preceding::DIRECTIVE[ @name='var_replace' and ./CLAUSE/@name='var' and ./CLAUSE/ARG[1]/@value=current()/@name ][1]/CLAUSE/ARG[2]/@value"/>
							</xsl:attribute>
							<xsl:apply-templates/>
						</xsl:copy>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>

			<!--	!$xev scalar2array1_varref start(スカラ変数名,配列サイズ,添字)
			
				変数の参照をスカラ変数から1次元配列に置き換える
				
				変数の変換は指定範囲内を行う
					!$xev scalar2array1_varref start
				 	      ～この間変数を変換する～
					!$xev end scalar2array1_varref
			-->
			<xsl:when test="./@name=//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start' and @specified='true']/ARG[1]/@value">
				<xsl:choose>
					<!-- 
						現在ノードより文書順で前にある最も近いノードが、
						範囲指定終了【!$xev end scalar2array1_varref(スカラ変数,サイズ,添字)】の場合
						変換処理を行わない【範囲外】
					-->
					<xsl:when test="preceding::DIRECTIVE[./CLAUSE/ARG[1]/@value=current()/@name and @name='end' or @name='scalar2array1_varref'][1]/CLAUSE/@name='scalar2array1_varref'">
						<xsl:copy>
							<xsl:copy-of select="@*"/>
							<xsl:apply-templates/>
						</xsl:copy>
					</xsl:when>

					<!--
						変換の有効範囲内の場合、スカラ変数名＋"_tmp" の１次元変数として参照する
					-->
					<xsl:otherwise>
						<xsl:element name="SgPntrArrRefExp">
							<!-- １次元変数名を設定する -->
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">
									<xsl:value-of select="concat(./@name,'_tmp')"/>
								</xsl:attribute>
							</xsl:element>

							<!-- 配列の添字を設定する -->
							<xsl:element name="SgExprListExp">
								<xsl:element name="SgVarRefExp">
									<xsl:attribute name="name">
										<xsl:value-of select="preceding::DIRECTIVE[ @name='scalar2array1_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/@name ][1]/CLAUSE/ARG[3]/@value"/>
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
			<!--	!$xev array_dim_chg strat(配列名,次元番号,次元番号,...)
			
				多次元配列の参照を指定次元番号順にする

				次元の入れ替えは指定範囲内を行う
					!$xev array_dim_chg strat
				 	      ～この間次元入れ替えする～
					!$xev end array_dim_chg
			-->
			<xsl:when test="preceding-sibling::SgVarRefExp[1]/@name=preceding::DIRECTIVE[@name='array_dim_chg']/CLAUSE[@name='start' and @specified='true']/ARG[1]/@value">
				<xsl:choose>
					<!-- 
						現在ノードより文書順で前にある最も近いノードが、
						範囲指定終了【!$xev end array_dim_chg(配列名)】の場合
						変換しない
					-->
					<xsl:when test="preceding::DIRECTIVE[./CLAUSE/ARG[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name and @name='end' or @name='array_dim_chg'][1]/CLAUSE/@name='array_dim_chg'">
						<xsl:copy>
							<xsl:copy-of select="@*"/>
							<xsl:apply-templates/>
						</xsl:copy>
					</xsl:when>

					<!-- 
						現在ノードより文書順で前にある【!$xev array_dim_chg start(配列名,次元番号,次元番号,次元番号)】がある場合
						次元入れ替えする
					-->
					<xsl:otherwise>
						<!-- カレントノード【SgExprListExp】を退避 -->
						<xsl:param name="expr-list" select="."/>
						<!-- 指定された次元番号のノードを取り出す -->
						<xsl:copy>
							<!-- ディレクティブ【array_dim_chg】の VARARG/ARG 分、次元の並び変えを行う -->
							<xsl:for-each select="preceding::DIRECTIVE[ @name='array_dim_chg' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name ][1]/CLAUSE/VARARG/ARG">
								<!-- 次元番号を取り出す -->
								<xsl:variable name="idx" select="number(@value)"/>
								<!-- テンプレートを使用して、指定された次元番号のノードを取り出す -->
								<xsl:apply-templates select="$expr-list/*[ $idx ]"/>
							</xsl:for-each>

						</xsl:copy>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>



			<!--	!$xev array1to2_varref start(変数名,添字,添字)
			
				1次元配列の参照を2次元配列に置き換える

				変数の変換は指定範囲内を行う
					!$xev array1to2_varref start
				 	      ～この間変数を変換する～
					!$xev end array1to2_varref
			-->
			<xsl:when test="preceding-sibling::SgVarRefExp[1]/@name=preceding::DIRECTIVE[@name='array1to2_varref']/CLAUSE[@name='start' and @specified='true']/ARG[1]/@value">
				<xsl:choose>
					<!-- 
						現在ノードより文書順で前にある最も近いノードが、
						範囲指定終了【!$xev end array1to2_varref(変数名,添字,添字)】の場合
						ディフォルト値を設定する
					-->
					<xsl:when test="preceding::DIRECTIVE[./CLAUSE/ARG[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name and @name='end' or @name='array1to2_varref'][1]/CLAUSE/@name='array1to2_varref'">
						<xsl:choose>
							<!--
								ディフォルト値の指定【!$xev array1to2_varref start(変数名,添字,添字,default)】がある場合、ディフォルト値を設定する
							-->
							<xsl:when test="//DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name]/CLAUSE/ARG[4]/@value='default'">
								<xsl:copy>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name and ./CLAUSE/ARG[4]/@value='default']/CLAUSE/ARG[2]/@value"/>

										</xsl:attribute>
									</xsl:element>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
											<xsl:value-of select="//DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name and ./CLAUSE/ARG[4]/@value='default']/CLAUSE/ARG[3]/@value"/>
										</xsl:attribute>
									</xsl:element>
								</xsl:copy>

							</xsl:when>

							<!--
								ディフォルト値の指定がない場合、変換しないでそのまま設定する
							-->
							<xsl:otherwise>
								<xsl:copy>
									<xsl:copy-of select="@*"/>
									<xsl:apply-templates/>
								</xsl:copy>
							</xsl:otherwise>
						</xsl:choose>
					</xsl:when>

					<!-- 
						現在ノードより文書順で前にある【!$xev array1to2_varref start(変数名,添字,添字)】の
						添字設定する
					-->
					<xsl:otherwise>
						<xsl:copy>
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">
									<xsl:value-of select="preceding::DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name][1]/CLAUSE/ARG[2]/@value"/>
								</xsl:attribute>
							</xsl:element>
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">
									<xsl:value-of select="preceding::DIRECTIVE[ @name='array1to2_varref' and ./CLAUSE/@name='start' and ./CLAUSE/ARG[1]/@value=current()/preceding-sibling::SgVarRefExp[1]/@name][1]/CLAUSE/ARG[3]/@value"/>
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
			<!--	!$xev scalar2array1_varref start(スカラ変数,サイズ,添字)
			
				スカラ変数のワーク用１次元配列を追加する
					Ex.  INTEGER hoge　→　INTEGER hoge,hoge_tmp(サイズ)
			-->
			<!-- 指定スカラ変数の宣言の場合 -->
			<xsl:when test="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start' and @specified='true']/ARG[1]/@value=./@name">
				<!-- 既存の変数宣言を出力する -->
				<xsl:copy-of select="."/>
				<!-- 既存の変数名＋"_tmp" の１次元配列宣言を行う -->
				<xsl:copy>
					<!-- １次元配列名 -->
					<xsl:attribute name="name">
							<xsl:value-of select="concat(./@name,'_tmp')"/>
					</xsl:attribute>
					<!-- １次元配列名 -->
					<xsl:element name="SgArrayType">
						<xsl:attribute name="index">""</xsl:attribute>
						<xsl:attribute name="rank">1</xsl:attribute>
						<xsl:attribute name="type">
							<xsl:value-of select="local-name(./*[1])"/>
						</xsl:attribute>
					
						<xsl:copy-of select="./*[1]"/>

						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[1][@value=current()/@name]/following-sibling::*[1]/@value"/>
							</xsl:attribute>
						</xsl:element>
					</xsl:element>
				</xsl:copy>
			</xsl:when>


			<!--	!$xev array1to2 type(1次元配列名,サイズ,サイズ)
			
				1次元配列の宣言を2次元配列に置き換える
			-->
			<xsl:when test="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type' and @specified='true']/ARG[1]/@value=./@name">
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
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type']/ARG[1][@value=current()/@name]/following-sibling::*[1]/@value"/>
							</xsl:attribute>
						</xsl:element>
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type']/ARG[1][@value=current()/@name]/following-sibling::*[2]/@value"/>
							</xsl:attribute>
						</xsl:element>
					</xsl:element>
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


	<!-- SgVariableDeclaration -->
	<xsl:template match="SgVariableDeclaration">
		<xsl:choose>
			<!--	!$xev one_loop_copy flag( サイズ,サイズ, ... )

				子ノードに【!$xev one_loop_copy flag】が存在する場合、
				コピー有無フラグを追加宣言する
			-->
			<xsl:when test=".=//SgVariableDeclaration[last()]" >
				<xsl:choose>
					<xsl:when test="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag' and @specified='true']" >
						<xsl:copy>
							<xsl:copy-of select="@*"/>
							<xsl:apply-templates/>
						</xsl:copy>
						<xsl:copy>
							<SgInitializedName name="xev_flg">
								<SgArrayType>
									<xsl:attribute name="index">""</xsl:attribute>
									<xsl:attribute name="rank">
										<xsl:value-of select="count(//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag']/VARARG/ARG)"/>
									</xsl:attribute>
									<xsl:attribute name="type">"SgTypeInt"</xsl:attribute>
									<SgTypeInt/>

									<xsl:for-each select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag']/VARARG/ARG">
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
			<!--	!$xev statement remove

				直前に'statement remove'がある場合、この statement を削除する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='statement']/CLAUSE[@name='remove' and @specified='true']">

				<xsl:comment>
					remove statement
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

	
	<!-- SgWhileStmt -->
	<xsl:template match="SgWhileStmt">
		<xsl:choose>
			<!--	!$xev while2do replace(変数,初期値,最終値[,刻み幅])

				直前に'while2do replace'がある場合、この【WHILE文】を【do文】に置き換える
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace' and @specified='true']">
				<xsl:element name="SgFortranDo">

					<!-- 【WHILE文】の属性をそのまま使用する -->
					<xsl:attribute name="end">
						<xsl:value-of select="@end"/>
					</xsl:attribute>
					<xsl:attribute name="nlabel">
						<xsl:value-of select="@nlabel"/>
					</xsl:attribute>
					<xsl:attribute name="slabel">
						<xsl:value-of select="@slabel"/>
					</xsl:attribute>

					<!-- 【変数=初期値】 -->
					<xsl:element name="SgAssignOp">
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/ARG[1]/@value"/>
							</xsl:attribute>
						</xsl:element>
						<xsl:element name="SgIntVal">
							<xsl:attribute name="value">
								<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/ARG[2]/@value"/>
							</xsl:attribute>
						</xsl:element>
					</xsl:element>

					<!-- 【最終値】 -->
					<xsl:element name="SgVarRefExp">
						<xsl:attribute name="name">
							<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/ARG[3]/@value"/>
						</xsl:attribute>
					</xsl:element>

					<!-- 【刻み幅】 -->
					<xsl:choose>
						<!-- 【刻み幅】の指定があるとき -->
						<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/ARG[4]/@specified='true'">
							<xsl:element name="SgIntVal">
								<xsl:attribute name="value">
									<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace']/ARG[4]/@value"/>
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
			<!--	!$xev fortrando replace(変数,初期値,最終値[,刻み幅])

				直前に'fortrando replace'がある場合、DO文を置き替える
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='fortrando']/CLAUSE[@name='replace' and @specified='true']">
				<xsl:element name="SgFortranDo">
					<!-- 属性をそのまま使用する -->
					<xsl:copy-of select="@*"/>

					<!-- 【変数=初期値】 -->
					<xsl:element name="SgAssignOp">
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
								<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='fortrando']/CLAUSE[@name='replace']/ARG[1]/@value"/>
							</xsl:attribute>
					</xsl:element>
					<xsl:element name="SgIntVal">
						<xsl:attribute name="value">
							<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='fortrando']/CLAUSE[@name='replace']/ARG[2]/@value"/>
						</xsl:attribute>
					</xsl:element>
					</xsl:element>

					<!-- 【最終値】 -->
					<xsl:element name="SgVarRefExp">
						<xsl:attribute name="name">
							<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='fortrando']/CLAUSE[@name='replace']/ARG[3]/@value"/>
						</xsl:attribute>
					</xsl:element>

					<!-- 【刻み幅】 -->
					<xsl:choose>
						<!-- 【刻み幅】の指定があるとき -->
						<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='fortrando']/CLAUSE[@name='replace']/ARG[4]/@specified='true'">
							<xsl:element name="SgIntVal">
								<xsl:attribute name="value">
									<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='fortrando']/CLAUSE[@name='replace']/ARG[4]/@value"/>
								</xsl:attribute>
							</xsl:element>
						</xsl:when>

						<!-- 【刻み幅】の指定がないとき、ディフォルト値(1)を設定する -->
						<xsl:otherwise>
							<SgNullExpression/>
						</xsl:otherwise>
					</xsl:choose>

					<!-- 
						【WHILE文】の処理内容【SgBasicBlock】をテンプレートを使用して複写する
			      		-->
					<xsl:apply-templates select="./SgBasicBlock"/>

				</xsl:element>
			</xsl:when>


			<!--	!$xev blocking block(変数1,サイズ1,変数2,サイズ2)

				ＤＯ文の直前に'!$xev blocking block'がある場合、 変数１，２の
				参照する領域をブロック単位になるようＤＯ文を変更する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='blocking']/CLAUSE[@name='block' and @specified='true']">
				<!--
					ブロック指定の変数、サイズを変数に取り出す
				-->
				<xsl:param name="var1_nam" select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='blocking']/CLAUSE[@name='block']/ARG[1]/@value"/>
				<xsl:param name="var1_siz" select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='blocking']/CLAUSE[@name='block']/ARG[2]/@value"/>
				<xsl:param name="var1_lop" select="concat($var1_nam,$var1_nam)"/>
				<xsl:param name="var2_nam" select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='blocking']/CLAUSE[@name='block']/ARG[3]/@value"/>
				<xsl:param name="var2_siz" select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='blocking']/CLAUSE[@name='block']/ARG[4]/@value"/>
				<xsl:param name="var2_lop" select="concat($var2_nam,$var2_nam)"/>

				<!--
					２重ＤＯ文を追加し、その中に既存のＤＯ文を変換して出力する
				-->
				<SgFortranDo end="1" nlabel="" slabel="" style="0">
					<!-- 初期値 -->
					<SgAssignOp>
						<SgVarRefExp>
							<xsl:attribute name="name">
								<xsl:value-of select="$var1_lop"/>
							</xsl:attribute>
						</SgVarRefExp>
						<SgIntVal value="1"/>
					</SgAssignOp>
					<!-- 最終値 -->
					<!-- 変数１がループ変数になっているＤＯ文の最終値を取り出す -->
					<xsl:copy-of select="//SgFortranDo[./SgAssignOp/SgVarRefExp/@name=$var1_nam]/*[2]"/>
					<!-- 刻み幅 -->
					<SgIntVal>
						<xsl:attribute name="value">
							<xsl:value-of select="$var1_siz"/>
						</xsl:attribute>
					</SgIntVal>
				        <SgBasicBlock>
						<SgFortranDo end="1" nlabel="" slabel="" style="0">
							<!-- 初期値 -->
							<SgAssignOp>
								<SgVarRefExp>
									<xsl:attribute name="name">
										<xsl:value-of select="$var2_lop"/>
									</xsl:attribute>
								</SgVarRefExp>
								<SgIntVal value="1"/>
							</SgAssignOp>
							<!-- 最終値 -->
							<!-- 変数２がループ変数になっているＤＯ文の最終値を取り出す -->
							<xsl:copy-of select="//SgFortranDo[./SgAssignOp/SgVarRefExp/@name=$var2_nam]/*[2]"/>
							<!-- 刻み幅 -->
							<SgIntVal>
								<xsl:attribute name="value">
									<xsl:value-of select="$var2_siz"/>
								</xsl:attribute>
							</SgIntVal>

				        		<SgBasicBlock>
			  					<!-- 既存のＤＯ文の初期値、最終値を変換して出力する -->
								<xsl:apply-templates select="." mode="blocking">
									<xsl:with-param name="var1_nam" select="$var1_nam"/>
									<xsl:with-param name="var1_lop" select="$var1_lop"/>
									<xsl:with-param name="var1_siz" select="$var1_siz - 1"/>
									<xsl:with-param name="var2_nam" select="$var2_nam"/>
									<xsl:with-param name="var2_lop" select="$var2_lop"/>
									<xsl:with-param name="var2_siz" select="$var2_siz - 1"/>
								</xsl:apply-templates>
				        		</SgBasicBlock>
						</SgFortranDo>
				        </SgBasicBlock>
				</SgFortranDo>
			</xsl:when>


			<!--	!$xev loop_roll dim(次元no,変数,初期値,最終値)

				直前に'loop_roll dim'がある場合、DO文を追加し
				実行行の指定次元の値を指定変数に置き換える
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_roll']/CLAUSE[@name='dim' and @specified='true']">
				<xsl:param name="idx"/>
				<xsl:param name="var"/>
				<!-- 変換内容を、tmp に格納する -->
  				<xsl:variable name="tmp">
					<SgFortranDo end="1" nlabel="" slabel="" style="0">
						<SgAssignOp>
							<SgVarRefExp>
								<xsl:attribute name="name">
									<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_roll']/CLAUSE[@name='dim']/ARG[2]/@value"/>
								</xsl:attribute>
							</SgVarRefExp>
							<SgIntVal>
								<xsl:attribute name="value">
									<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_roll']/CLAUSE[@name='dim']/ARG[3]/@value"/>
								</xsl:attribute>
							</SgIntVal>
						</SgAssignOp>
						<SgVarRefExp>
							<xsl:attribute name="name">
								<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_roll']/CLAUSE[@name='dim']/ARG[4]/@value"/>
							</xsl:attribute>
						</SgVarRefExp>
						<SgNullExpression/>
					        <SgBasicBlock>
							<xsl:apply-templates select="." mode="loop_roll">
								<xsl:with-param name="idx" select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_roll']/CLAUSE[@name='dim']/ARG[1]/@value"/>
								<xsl:with-param name="var" select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_roll']/CLAUSE[@name='dim']/ARG[2]/@value"/>
							</xsl:apply-templates>
					        </SgBasicBlock>
					</SgFortranDo>
  				</xsl:variable>
				<!-- 変換内容(tmp)を、再変換して出力する -->

			  	<xsl:apply-templates select="exslt:node-set($tmp)"/>
			</xsl:when>

			<!--	!$xev loop_unroll step(刻み幅)

				直前に'loop_unroll step'がある場合、直下のDO文の刻み幅を変更する
				実行行を刻み幅分実行する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_unroll']/CLAUSE[@name='step' and @specified='true']">
				<xsl:copy>
					<xsl:copy-of select="@*"/>

					<!-- 変数 初期値 -->
					<xsl:copy-of select="./*[1]"/>
					<!-- 最終値 -->
					<xsl:copy-of select="./*[2]"/>
					<!-- 刻み幅 -->
					<xsl:element name="SgIntVal">
						<xsl:attribute name="value">
							<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_unroll']/CLAUSE[@name='step']/ARG[1]/@value"/>
						</xsl:attribute>
					</xsl:element>
					
  					<!-- multi-pass
						変換した内容を(tmp)に格納し
						その(tmp)内容を再変換する
					-->
					<!-- 変換内容を、tmp に格納する -->
  					<xsl:variable name="tmp">
						<xsl:apply-templates select="./SgBasicBlock" mode="loop_unroll">
							<xsl:with-param name="max" select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_unroll']/CLAUSE[@name='step']/ARG[1]/@value"/>
							<xsl:with-param name="var" select="./SgAssignOp/SgVarRefExp/@name"/>
						</xsl:apply-templates>
			  		</xsl:variable>
					<!-- 変換内容(tmp)を、再変換して出力する -->
			  		<xsl:apply-templates select="exslt:node-set($tmp)"/>

				</xsl:copy>

				<!-- Epilog 処理 -->
				<xsl:copy>
					<xsl:copy-of select="@*"/>

					<!-- 変数 初期値 -->
					<SgAssignOp>
						<xsl:copy-of select="./*[1]/SgVarRefExp"/>
						<SgSubtractOp>
							<xsl:copy-of select="./*[2]"/>
							<SgFunctionCallExp>
								<SgFunctionRefExp symbol="mod"/>
								<SgExprListExp>
									<xsl:copy-of select="./*[2]"/>
									<xsl:element name="SgIntVal">
										<xsl:attribute name="value">
											<xsl:value-of select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_unroll']/CLAUSE[@name='step']/ARG[1]/@value"/>
										</xsl:attribute>
									</xsl:element>
								</SgExprListExp>
							</SgFunctionCallExp>
						</SgSubtractOp>
					</SgAssignOp>
					<!-- 最終値 -->
					<xsl:copy-of select="./*[2]"/>
					<!-- 刻み幅 -->
					<xsl:copy-of select="./*[3]"/>
					
					<!-- 処理内容 -->
					<xsl:apply-templates select="./SgBasicBlock" mode="loop_unroll_Epilog"/>
				</xsl:copy>
			</xsl:when>

			<!--	!pragma xev one_loop_copy flag(n,n,n)

				直前に'one_loop_copy flag'がある場合、直下の多重ループコピーを
				１重ループのコピーに変換する
			-->
			<xsl:when test="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag' and @specified='true']">

				<!--
					マスクフラグを初期化する
				-->
				<xsl:param name="idx1" select="0"/>
				<xsl:apply-templates select="." mode="one_loop_copy_0">
					<xsl:with-param name="idx1" select="$idx1 + 1"/>
				</xsl:apply-templates>


				<!--
					マスクフラグを設定する
				-->
				<xsl:apply-templates select="." mode="one_loop_copy_1"/>

				<!--
					マスクフラグを参照しデータをコピーする
				-->
				<xsl:param name="idx2" select="0"/>
				<xsl:apply-templates select="." mode="one_loop_copy_c">
					<xsl:with-param name="idx2" select="$idx2 + 1"/>
				</xsl:apply-templates>

			</xsl:when>


			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration">
		<xsl:choose>
			<!--	!$xev array_dim_chg start(a1,3,2,1)
				ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='array_dim_chg']/CLAUSE[@name='start' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration var_replace var
				</xsl:comment>
			</xsl:when>

			<!--	!$xev var_replace var(i,j)
				ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='var_replace']/CLAUSE[@name='var' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration var_replace var
				</xsl:comment>
			</xsl:when>

			<!--	!$xev fortrando replace(k,1,n3)
				ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='fortrando']/CLAUSE[@name='replace' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration fortrando replace
				</xsl:comment>
			</xsl:when>

			<!--	!$xev blocking block(k,8,i,8)
				ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='blocking']/CLAUSE[@name='block' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration blocking
				</xsl:comment>
			</xsl:when>

			<!--	!$xev loop_roll dimp
				ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='loop_roll']/CLAUSE[@name='dim' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration loop_roll
				</xsl:comment>
			</xsl:when>

			<!--	!$xev loop_unroll step(刻み幅)
				ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='loop_unroll']/CLAUSE[@name='step' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration loop_unroll
				</xsl:comment>
			</xsl:when>

			<!--	!$xev one_loop_copy flag(n,n,n)
				ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration one_loop_copy
				</xsl:comment>
			</xsl:when>

			<!--	!$xev scalar2array1_varref start(変数名,サイズ,添字)
				追加した１次元配列を初期化する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start' and @specified='true']">
          
				<SgFortranDo end="1" nlabel="" slabel="" style="0">
					<SgAssignOp>
						<SgVarRefExp>
							<xsl:attribute name="name">
								<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[3]/@value"/>
							</xsl:attribute>
						</SgVarRefExp>
						<SgIntVal value="1"/>
					</SgAssignOp>
					<SgVarRefExp>
						<xsl:attribute name="name">
							<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[2]/@value"/>
						</xsl:attribute>
					</SgVarRefExp>
					<SgNullExpression/>
					<SgBasicBlock>
						<SgExprStatement>
							<SgAssignOp>
								<SgPntrArrRefExp>
									<SgVarRefExp>
										<xsl:attribute name="name">
											<xsl:value-of select="concat(./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[1]/@value,'_tmp')"/>
										</xsl:attribute>
									</SgVarRefExp>
									<SgExprListExp>
										<SgVarRefExp>
											<xsl:attribute name="name">
												<xsl:value-of select="./SgPragma/DIRECTIVE[@name='scalar2array1_varref']/CLAUSE[@name='start']/ARG[3]/@value"/>
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

			<!--	!$xev array1to2 varref(1次元配列名,添字,添字)
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='array1to2_varref']/CLAUSE[@name='start' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev array1to2 type(1次元配列名,サイズ,サイズ)
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='array1to2']/CLAUSE[@name='type' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev dir add
				ディレクティブを追加する	
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='add' and @specified='true']">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='add']/VARARG/ARG">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>

			<!--	!$xev dir replace
				ディレクティブを置換する	
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace' and @specified='true']">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='replace']/VARARG/ARG">
								<xsl:value-of select="@value" /><xsl:text> </xsl:text>
							</xsl:for-each>
						</xsl:attribute>
					</xsl:element>
				</xsl:element>
			</xsl:when>

			<!--	!$xev dir remove
                                ディレクティブを削除する
                        -->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='remove' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration
				</xsl:comment>
			</xsl:when>

			<!--	!$xev dir append( [文字列] )
                                ディレクティブ行に[文字列]を追加する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append'and @specified='true']">
				<xsl:element name="SgPragmaDeclaration">
					<xsl:element name="SgPragma">
						<xsl:attribute name="pragma">
							<!--
								追加するもとのディレクティブ行の内容を取り出す

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
							<xsl:for-each select="./SgPragma/DIRECTIVE[@name='dir']/CLAUSE[@name='append']/VARARG/ARG">
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


			<!--	!$xev statement add(... )
				指定文字列をXMLに出力する
			-->
                        <xsl:when test="./SgPragma/DIRECTIVE[@name='statement']/CLAUSE[@name='add' and @specified='true']">
                        	<xsl:for-each select="./SgPragma/DIRECTIVE[@name='statement']/CLAUSE[@name='add']/VARARG/ARG">
					<xsl:value-of select="@value" /><xsl:text> </xsl:text>
				</xsl:for-each>
			</xsl:when>

			<!--	!$xev statement remove
                                ディレクティブを削除する
 			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='statement']/CLAUSE[@name='remove' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration statement remove
				</xsl:comment>
			</xsl:when>

			<!--	!$xev while2do replace
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='while2do']/CLAUSE[@name='replace' and @specified='true']">
				<xsl:comment>
					SgPragmaDeclaration while2do replace
				</xsl:comment>
			</xsl:when>

			<!--	!$xev end xxxxx
                                ディレクティブを削除する
			-->
			<xsl:when test="./SgPragma/DIRECTIVE[@name='end']/CLAUSE[@specified='true']">
				<xsl:comment>
					SgPragmaDeclaration end xxxx
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
					remove PreprocessingInfo
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
					remove PreprocessingInfo
				</xsl:comment>
			</xsl:when>

			<!--直前に'!$xev dir replace'がある場合、この行を削除する -->
			<xsl:when test="contains(preceding-sibling::PreprocessingInfo[1],'!$xev dir replace')">
				<xsl:comment>
					remove PreprocessingInfo
				</xsl:comment>
			</xsl:when>

			<!--直前に'!$xev dir remove'がある場合、この行を削除する -->
			<xsl:when test="contains(preceding-sibling::PreprocessingInfo[1],'!$xev dir remove')">
				<xsl:comment>
					remove PreprocessingInfo
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



<!-- これ以降は個別 XSL +++++++++++++++++++++++++++++++++++++++++++++++++++++-->

<!-- [blocking] +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++-->
	<!--	SgFortranDo
		既存ＤＯ文の【初期値】，【最終値】を変換する
	-->
	<xsl:template match="SgFortranDo" mode="blocking">
		<xsl:param name="var1_nam"/>	<!-- 変数１の変数名             -->
		<xsl:param name="var1_lop"/>	<!-- 変数１のループ変数名       -->
		<xsl:param name="var1_siz"/>	<!-- 変数１のブロックサイズ－１ -->
		<xsl:param name="var2_nam"/>	<!-- 変数２の変数名             -->
		<xsl:param name="var2_lop"/>	<!-- 変数２のループ変数名       -->
		<xsl:param name="var2_siz"/>	<!-- 変数２のブロックサイズ－１ -->

		<xsl:choose>
			<!--
				既存ＤＯ文初期値の変数名が変数１と一致した場合
				初期値【$var1_lop】、最終値【min(既存の最終値,$var1_lop+$var1_siz)】を設定する
			-->
			<xsl:when test="./SgAssignOp/SgVarRefExp/@name=$var1_nam">
				<xsl:copy>
					<!-- 【属性】 -->
					<xsl:copy-of select="@*"/>

					<!-- 【初期値】 -->
					<SgAssignOp>
						<!-- 変数はそのまま使用する -->
						<xsl:copy-of select="./SgAssignOp/SgVarRefExp[1]"/>
						<!-- 初期値は変数１のループ変数名($var1_lop)を設定する -->
						<SgVarRefExp>
							<xsl:attribute name="name">
								<xsl:value-of select="$var1_lop"/>
							</xsl:attribute>
						</SgVarRefExp>
					</SgAssignOp>

					<!-- 【最終値】min(もとの最終値,ループ変数名($var1_lop)＋ブロックサイズ($var1_siz)) -->
					<SgFunctionCallExp>
						<SgFunctionRefExp symbol="min"/>
						<SgExprListExp>
							<!-- もとの最終値 -->
							<xsl:copy-of select="./*[2]"/>
							<SgAddOp>
								<SgVarRefExp>
									<xsl:attribute name="name">
										<xsl:value-of select="$var1_lop"/>
									</xsl:attribute>
								</SgVarRefExp>
								<SgIntVal>
									<xsl:attribute name="value">
										<xsl:value-of select="$var1_siz"/>
									</xsl:attribute>
								</SgIntVal>
							</SgAddOp>
						</SgExprListExp>
					</SgFunctionCallExp>

					<!-- 【刻み幅】 もとの刻み幅を使用する-->
					<xsl:copy-of select="./*[3]"/>

					<!-- 【SgBasicBlock】 -->
					<xsl:apply-templates select="./SgBasicBlock" mode="blocking">
						<xsl:with-param name="var1_nam" select="$var1_nam"/>
						<xsl:with-param name="var1_lop" select="$var1_lop"/>
						<xsl:with-param name="var1_siz" select="$var1_siz"/>
						<xsl:with-param name="var2_nam" select="$var2_nam"/>
						<xsl:with-param name="var2_lop" select="$var2_lop"/>
						<xsl:with-param name="var2_siz" select="$var2_siz"/>
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:when>

			<!--
				既存ＤＯ文初期値の変数名が変数２と一致した場合
				初期値【$var2_lop】、最終値【min(既存の最終値,$var2_lop+$var2_siz)】を指定値に変換する
			-->
			<xsl:when test="./SgAssignOp/SgVarRefExp/@name=$var2_nam">
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<!-- 【初期値】 -->
					<SgAssignOp>
						<!-- 変数はそのまま使用する -->
						<xsl:copy-of select="./SgAssignOp/SgVarRefExp[1]"/>
						<!-- 初期値は変数１のループ変数名($var1_lop)を設定する -->
						<SgVarRefExp>
							<xsl:attribute name="name">
								<xsl:value-of select="$var2_lop"/>
							</xsl:attribute>
						</SgVarRefExp>
					</SgAssignOp>

					<!-- 【最終値】min(もとの最終値,ループ変数名($var2_lop)＋ブロックサイズ($var2_siz)) -->
					<SgFunctionCallExp>
						<SgFunctionRefExp symbol="min"/>
						<SgExprListExp>
							<!-- もとの最終値 -->
							<xsl:copy-of select="./*[2]"/>
							<SgAddOp>
								<SgVarRefExp>
									<xsl:attribute name="name">
										<xsl:value-of select="$var2_lop"/>
									</xsl:attribute>
								</SgVarRefExp>
								<SgIntVal>
									<xsl:attribute name="value">
										<xsl:value-of select="$var2_siz"/>
									</xsl:attribute>
								</SgIntVal>
							</SgAddOp>
						</SgExprListExp>
					</SgFunctionCallExp>

					<!-- 【刻み幅】 もとの刻み幅を使用する-->
					<xsl:copy-of select="./*[3]"/>

					<!-- 【SgBasicBlock】 -->
					<xsl:apply-templates select="./SgBasicBlock" mode="blocking">
						<xsl:with-param name="var1_nam" select="$var1_nam"/>
						<xsl:with-param name="var1_lop" select="$var1_lop"/>
						<xsl:with-param name="var1_siz" select="$var1_siz"/>
						<xsl:with-param name="var2_nam" select="$var2_nam"/>
						<xsl:with-param name="var2_lop" select="$var2_lop"/>
						<xsl:with-param name="var2_siz" select="$var2_siz"/>
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:when>

			<!--
				上記以外はパラメータを引き継ぎ、blockingモードのテンプレートで設定する
			-->
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"/>
					<xsl:apply-templates mode="blocking">
						<xsl:with-param name="var1_nam" select="$var1_nam"/>
						<xsl:with-param name="var1_lop" select="$var1_lop"/>
						<xsl:with-param name="var1_siz" select="$var1_siz"/>
						<xsl:with-param name="var2_nam" select="$var2_nam"/>
						<xsl:with-param name="var2_lop" select="$var2_lop"/>
						<xsl:with-param name="var2_siz" select="$var2_siz"/>
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo" mode="blocking">
		<xsl:choose>
			<xsl:when test="contains(.,'!$xev')" >
				<xsl:comment>
					remove PreprocessingInfo
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
	<!--
		上記以外はパラメータを引き継ぎ、blockingモードのテンプレートで設定する
	-->
	<xsl:template match="*" mode="blocking">
		<xsl:param name="var1_nam"/>
		<xsl:param name="var1_lop"/>
		<xsl:param name="var1_siz"/>
		<xsl:param name="var2_nam"/>
		<xsl:param name="var2_lop"/>
		<xsl:param name="var2_siz"/>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="blocking">
				<xsl:with-param name="var1_nam" select="$var1_nam"/>
				<xsl:with-param name="var1_lop" select="$var1_lop"/>
				<xsl:with-param name="var1_siz" select="$var1_siz"/>
				<xsl:with-param name="var2_nam" select="$var2_nam"/>
				<xsl:with-param name="var2_lop" select="$var2_lop"/>
				<xsl:with-param name="var2_siz" select="$var2_siz"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
<!--+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++-->


<!-- [loop_roll] ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++-->

	<!--
		最初の【SgExprStatement】行の場合
		テンプレートを使って出力する
	-->
	<xsl:template match="SgExprStatement[1]" mode="loop_roll">
		<xsl:param name="idx"/>
		<xsl:param name="var"/>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates  mode="loop_roll">
				<xsl:with-param name="idx" select="$idx"/>
				<xsl:with-param name="var" select="$var"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
	<!--
		最初でない【SgExprStatement】行の場合は出力しない
	-->
	<xsl:template match="SgExprStatement" mode="loop_roll">
	</xsl:template>


	<!--
		配列参照の指定次元の値を、指定変数に置き換える
	-->
	<xsl:template match="SgExprListExp" mode="loop_roll">
		<xsl:param name="idx"/>		<!-- 次元no     -->
		<xsl:param name="var"/>		<!-- 指定変数名 -->

		<xsl:copy>
			<xsl:copy-of select="@*" />
			<!--
				配列次元数分、処理を行う
			-->
			<xsl:for-each select="./*">

				<!-- 
			     		処理している変数の位置が、指定次元noのとき
			     		指定変数名に置き換える
			     	-->
				<xsl:if test="position()=$idx" >
					<SgVarRefExp>
						<xsl:attribute name="name">
							<xsl:value-of select="$var"/>
						</xsl:attribute>
					</SgVarRefExp>
  				</xsl:if>

				<!-- 上記以外はそのまま出力する -->
				<xsl:if test="position()!=$idx" >
					<xsl:apply-templates select="." mode="loop_roll">
						<xsl:with-param name="idx" select="$idx"/>
						<xsl:with-param name="var" select="$var"/>
					</xsl:apply-templates>
  				</xsl:if>
  			</xsl:for-each>

		</xsl:copy>
	</xsl:template>
	<xsl:template match="*" mode="loop_roll">
		<xsl:param name="idx"/>		<!-- 次元no -->
		<xsl:param name="var"/>		<!-- 変数名 -->
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="loop_roll">
				<xsl:with-param name="idx" select="$idx"/>
				<xsl:with-param name="var" select="$var"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
<!--+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++-->


<!-- [loop_unroll] ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++-->
	<!--
		最後の【SgExprStatement】行の場合
	-->
	<xsl:template match="SgExprStatement[last()]" mode="loop_unroll">
		<xsl:param name="max"/>		<!-- STEP数 -->
		<xsl:param name="var"/>		<!-- 置き換える変数 -->

		<!-- 自分を出力する -->
		<xsl:apply-templates select="."/>

		<!-- コピーする【SgExprStatement】行を設定 -->
		<xsl:param name="copy-stm" select="../SgExprStatement"/>

		<!-- コピーする【SgExprStatement】行を設定 -->
		<xsl:for-each select="(//*)[position()&lt;$max]">
			<xsl:apply-templates select="$copy-stm" mode="loop_unroll_1">
				<xsl:with-param name="max" select="$max"/>
				<xsl:with-param name="var" select="$var"/>
				<xsl:with-param name="cnt" select="position()"/>
			</xsl:apply-templates>
  		</xsl:for-each>

	</xsl:template>
	<xsl:template match="*" mode="loop_unroll">
		<xsl:param name="max"/>
		<xsl:param name="var"/>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="loop_unroll">
				<xsl:with-param name="max" select="$max"/>
				<xsl:with-param name="var" select="$var"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo"  mode="loop_unroll">
	</xsl:template>
	<!--
	<xsl:template match="SgPragmaDeclaration[1]" mode="loop_unroll">
	</xsl:template>
	-->


	<xsl:template match="SgVarRefExp" mode="loop_unroll_1">
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

	<xsl:template match="*" mode="loop_unroll_1">
		<xsl:param name="max"/>
		<xsl:param name="var"/>
		<xsl:param name="cnt"/>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="loop_unroll_1">
				<xsl:with-param name="max" select="$max"/>
				<xsl:with-param name="var" select="$var"/>
				<xsl:with-param name="cnt" select="$cnt"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>


	<!-- SgPragmaDeclaration 削除 -->
	<xsl:template match="SgPragmaDeclaration" mode="loop_unroll_Epilog">
	</xsl:template>
	<!-- PreprocessingInfo 削除 -->
	<xsl:template match="PreprocessingInfo" mode="loop_unroll_Epilog">
	</xsl:template>
	<!-- その他は全て出力する -->
	<xsl:template match="*" mode="loop_unroll_Epilog">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="loop_unroll_Epilog"/>
		</xsl:copy>
	</xsl:template>
<!--+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++-->


<!-- [one_loop_copy] ++++++++++++++++++++++++++++++++++++++++++++++++++++++++-->
	<!-- 
		マスクフラグを初期化するテンプレート【ここから】 
		DO文をコピーして、マスクフラグ領域に０を設定するテンプレート
	-->
	<xsl:template match="SgFortranDo" mode="one_loop_copy_0">
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
					<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag']/VARARG/ARG[$idx1]/@value"/>
				</xsl:attribute>
			</SgVarRefExp>
			<!-- 刻み幅 -->
			<SgNullExpression/>
			<xsl:apply-templates mode="one_loop_copy_0">
				<xsl:with-param name="idx1" select="$idx1 + 1"/>
			</xsl:apply-templates>
			<!--
			<xsl:apply-templates select="./SgBasicBlock" mode="one_loop_copy_00"/>
			-->
		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgExprStatement" mode="one_loop_copy_0">
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
	<xsl:template match="*" mode="one_loop_copy_0">
		<xsl:param name="idx1" select="1"/>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="one_loop_copy_0">
				<xsl:with-param name="idx1" select="$idx1"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo"  mode="one_loop_copy_0">
	</xsl:template>
	<!-- remove SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration" mode="one_loop_copy_0">
	</xsl:template>


	<!-- 
		マスクフラグにコピーする領域を設定するテンプレート【ここから】 
		DO文をコピーして、マスクフラグ領域に１を設定する
	-->
	<xsl:template match="SgExprStatement" mode="one_loop_copy_1">
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
	<xsl:template match="*" mode="one_loop_copy_1">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="one_loop_copy_1"/>
		</xsl:copy>
	</xsl:template>
	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo"  mode="one_loop_copy_1">
	</xsl:template>
	<!-- remove SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration" mode="one_loop_copy_1">
	</xsl:template>
	<!-- SgFortranDoをコピーして、コピーする領域に１を設定する【ここまで】 -->


	<!-- 
		マスクフラグを参照してデータをコピーするテンプレート【ここから】 
		DO文をコピーして、マスクフラグ領域に０を設定するテンプレート
	-->
	<xsl:template match="SgFortranDo" mode="one_loop_copy_c">
		<!-- DO文の階層を変数に設定する -->
		<xsl:param name="idx2" select="1"/>
		<xsl:choose>
			<xsl:when test="$idx2=count(//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag']/VARARG/ARG)">
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
						<xsl:with-param name="lop" select="count(//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag']/VARARG/ARG)"/>
					</xsl:call-template>

					<!-- 刻み幅 -->
					<SgNullExpression/>
					<xsl:apply-templates select="./SgBasicBlock" mode="one_loop_copy_c">
						<xsl:with-param name="idx2" select="$idx2+1"/>
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:when>

			<xsl:otherwise>
				<xsl:apply-templates select="./SgBasicBlock" mode="one_loop_copy_c">
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
						<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag']/VARARG/ARG[1]/@value" />
					</xsl:attribute>
				</SgVarRefExp>
			</xsl:if>

			<SgVarRefExp>
				<xsl:attribute name="name">
					<xsl:value-of select="//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag']/VARARG/ARG[$lop]/@value" />
				</xsl:attribute>
			</SgVarRefExp>
		</SgMultiplyOp>
	</xsl:template>



	<xsl:template match="SgExprStatement" mode="one_loop_copy_c">

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
							<xsl:variable name="count" select="count(//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag']/VARARG/ARG)-1"/>
							<xsl:for-each select="(//*)[position()&lt;=$count]">
								<SgIntVal value="1"/>
  							</xsl:for-each>
						</SgExprListExp>
						-->
						<xsl:apply-templates select="./SgAssignOp/SgPntrArrRefExp/SgExprListExp" mode="one_loop_copy_c"/>
					</SgPntrArrRefExp>
					<SgIntVal value="1"/>
				</SgEqualityOp>
			</SgExprStatement>
			<SgBasicBlock>

				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="one_loop_copy_c"/>
				</xsl:copy>
			</SgBasicBlock>
		</SgIfStmt>
	</xsl:template>

	<xsl:template match="SgExprListExp" mode="one_loop_copy_c">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<SgVarRefExp>
				<xsl:attribute name="name">
					<xsl:value-of select="./SgVarRefExp[1]/@name" />
				</xsl:attribute>
			</SgVarRefExp>
			<xsl:variable name="count" select="count(//SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='one_loop_copy']/CLAUSE[@name='flag']/VARARG/ARG)-1"/>
			<xsl:for-each select="(//*)[position()&lt;=$count]">
				<SgIntVal value="1"/>
  			</xsl:for-each>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="*" mode="one_loop_copy_c">
		<xsl:param name="idx2" select="1"/>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="one_loop_copy_c">
				<xsl:with-param name="idx2" select="$idx2"/>
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo"  mode="one_loop_copy_c">
	</xsl:template>
	<!-- remove SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration" mode="one_loop_copy_c">
	</xsl:template>

<!--+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++-->

</xsl:stylesheet>
