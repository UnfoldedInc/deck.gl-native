// Copyright (c) 2020, Unfolded Inc
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef EXAMPLES_DECKGL_DATA_AIRPORTS_H
#define EXAMPLES_DECKGL_DATA_AIRPORTS_H

static auto heathrowFlights = R"JSON(
{"type":"mid","name":"Rotterdam The Hague","abbrev":"RTM","coordinates":[4.433844349628764,51.94913018993814]}
{"type":"major","name":"Liverpool John Lennon","abbrev":"LPL","coordinates":[-2.858620657849378,53.3363751054422]}
{"type":"major","name":"Newcastle Int'l","abbrev":"NCL","coordinates":[-1.71034578407216,55.03708486080194]}
{"type":"major","name":"Bremen","abbrev":"BRE","coordinates":[8.785861770313199,53.052287104156]}
{"type":"major","name":"Bristol Int'l","abbrev":"BRS","coordinates":[-2.710864691343084,51.386293418914825]}
{"type":"major","name":"Cardiff","abbrev":"CWL","coordinates":[-3.339568764291176,51.398622091101736]}
{"type":"major","name":"Edinburgh Int'l","abbrev":"EDI","coordinates":[-3.364284685135544,55.94855401136389]}
{"type":"mid","name":"Inverness","abbrev":"INV","coordinates":[-4.063591755871404,57.53950029234237]}
{"type":"mid","name":"Kirkwall","abbrev":"KOI","coordinates":[-2.901378495245076,58.954428878830335]}
{"type":"mid","name":"Sumburgh","abbrev":"LSI","coordinates":[-1.288060688387535,59.87668995989991]}
{"type":"major","name":"London Luton","abbrev":"LTN","coordinates":[-0.376227267397439,51.88029525709689]}
{"type":"major","name":"Malpensa","abbrev":"MXP","coordinates":[8.712959535024366,45.627440514038085]}
{"type":"major","name":"Nurnberg","abbrev":"NUE","coordinates":[11.077417973909554,49.49450521703444]}
{"type":"major","name":"Shannon","abbrev":"SNN","coordinates":[-8.922428855576856,52.69345371025315]}
{"type":"major","name":"Turin Int'l","abbrev":"TRN","coordinates":[7.644162303621326,45.19166007346416]}
{"type":"major","name":"Bordeaux","abbrev":"BOD","coordinates":[-0.701793449075243,44.83211086626738]}
{"type":"mid","name":"St Gatien","abbrev":"DOL","coordinates":[0.158653528230218,49.3616609986609]}
{"type":"mid","name":"Lille-Lesquin","abbrev":"LIL","coordinates":[3.105964997998134,50.57164239295805]}
{"type":"major","name":"Toulouse-Blagnac","abbrev":"TLS","coordinates":[1.373509185511526,43.63046256616008]}
{"type":"major","name":"Leeds Bradford","abbrev":"LBA","coordinates":[-1.659831067347459,53.869081947443426]}
{"type":"major","name":"Cork","abbrev":"ORK","coordinates":[-8.49014199983817,51.84854054199234]}
{"type":"mid","name":"Glasgow Prestwick","abbrev":"PIK","coordinates":[-4.610971639010681,55.50889181051419]}
{"type":"military mid","name":"Chalons Vatry","abbrev":"XCR","coordinates":[4.191119825742889,48.780394613856636]}
{"type":"major","name":"Luxembourg-Findel","abbrev":"LUX","coordinates":[6.216421217287303,49.634304092510156]}
{"type":"mid","name":"Deurne","abbrev":"ANR","coordinates":[4.450922773999087,51.18912850638063]}
{"type":"major","name":"London Gatwick","abbrev":"LGW","coordinates":[-0.162961639139456,51.15575675192744]}
{"type":"major","name":"Manchester Int'l","abbrev":"MAN","coordinates":[-2.273371590694273,53.3624896066518]}
{"type":"major","name":"Marseille Provence Airport","abbrev":"MRS","coordinates":[5.221379177203371,43.44106000164678]}
{"type":"major","name":"Birmingham Int'l","abbrev":"BHX","coordinates":[-1.733731704344524,52.4529085542838]}
{"type":"major","name":"Dresden","abbrev":"DRS","coordinates":[13.764967144004657,51.1250912428871]}
{"type":"mid","name":"Annemasse","abbrev":"QNJ","coordinates":[6.264910853641589,46.195728328626046]}
{"type":"mid","name":"Strasbourg","abbrev":"SXB","coordinates":[7.627841966889239,48.544696172175925]}
{"type":"major","name":"Cologne/Bonn","abbrev":"CGN","coordinates":[7.122359755245384,50.87825966294708]}
{"type":"major","name":"Stavanger Sola","abbrev":"SVG","coordinates":[5.629810329721797,58.882156484218456]}
{"type":"mid","name":"Bilbao","abbrev":"BIO","coordinates":[-2.906090116798052,43.30508298111946]}
{"type":"major","name":"Brussels","abbrev":"BRU","coordinates":[4.484640324082721,50.89729496415107]}
{"type":"major","name":"Belfast Int'l","abbrev":"BFS","coordinates":[-6.216169437349579,54.661557547010304]}
{"type":"major","name":"Orio Al Serio","abbrev":"BGY","coordinates":[9.6989176939974,45.66549805606944]}
{"type":"major","name":"Dusseldorf Int'l","abbrev":"DUS","coordinates":[6.764944466121738,51.27818204207739]}
{"type":"major","name":"Stuttgart","abbrev":"STR","coordinates":[9.193951089455355,48.69010513589127]}
{"type":"major","name":"Geneva","abbrev":"GVA","coordinates":[6.107945774236033,46.231009510157946]}
{"type":"mid","name":"Lyon-Saint Exupery","abbrev":"LYS","coordinates":[5.075944318134585,45.721018683466895]}
{"type":"major","name":"Glasgow Int'l","abbrev":"GLA","coordinates":[-4.431677969951068,55.864182857035445]}
{"type":"major","name":"Paris Orly","abbrev":"ORY","coordinates":[2.367379127837731,48.731303045805156]}
{"type":"major","name":"Copenhagen","abbrev":"CPH","coordinates":[12.649350868450766,55.62850172215277]}
{"type":"major","name":"Dublin","abbrev":"DUB","coordinates":[-6.243884910371393,53.42700828497003]}
{"type":"major","name":"Hamburg","abbrev":"HAM","coordinates":[10.005647830925,53.63200116408657]}
{"type":"major","name":"Franz-Josef-Strauss","abbrev":"MUC","coordinates":[11.788062719243683,48.353837396160834]}
{"type":"major","name":"Zurich Int'l","abbrev":"ZRH","coordinates":[8.562212795347643,47.45238950649154]}
{"type":"mid","name":"Gosselies","abbrev":"CRL","coordinates":[4.454373629816501,50.457129654956674]}
{"type":"major","name":"Munich Freight Terminal","abbrev":"MUCf","coordinates":[11.769482859365423,48.34979640783765]}
{"type":"major","name":"Schiphol","abbrev":"AMS","coordinates":[4.764376932328116,52.30893238898216]}
{"type":"major","name":"London Heathrow","abbrev":"LHR","coordinates":[-0.453156652063309,51.47099587999383]}
{"type":"major","name":"Linate","abbrev":"LIN","coordinates":[9.27996629691061,45.46039384562521]}
{"type":"major","name":"Charles de Gaulle Int'l","abbrev":"CDG","coordinates":[2.541867767394572,49.01442009693854]}
{"type":"major","name":"Berlin-Tegel Int'l","abbrev":"TXL","coordinates":[13.290309092507398,52.55442870441008]}
{"type":"major","name":"Frankfurt Int'l","abbrev":"FRA","coordinates":[8.571822869076074,50.050677089520725]}
)JSON";

#endif  // EXAMPLES_DECKGL_DATA_AIRPORTS_H
