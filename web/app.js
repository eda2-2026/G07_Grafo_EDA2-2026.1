/* ----------------------------------------------------------------------
 * Interface do grafo de rotas de combustivel.
 * Os dados (estados, arestas e rotas pre-calculadas) vem de rotas.js,
 * gerado pelo programa em C, na variavel global window.DADOS.
 * -------------------------------------------------------------------- */

const NOMES = {
  AC: "Acre", AL: "Alagoas", AM: "Amazonas", AP: "Amapá", BA: "Bahia",
  CE: "Ceará", DF: "Distrito Federal", ES: "Espírito Santo", GO: "Goiás",
  MA: "Maranhão", MG: "Minas Gerais", MS: "Mato Grosso do Sul",
  MT: "Mato Grosso", PA: "Pará", PB: "Paraíba", PE: "Pernambuco",
  PI: "Piauí", PR: "Paraná", RJ: "Rio de Janeiro", RN: "Rio Grande do Norte",
  RO: "Rondônia", RR: "Roraima", RS: "Rio Grande do Sul", SC: "Santa Catarina",
  SE: "Sergipe", SP: "São Paulo", TO: "Tocantins"
};

/* posicoes esquematicas (lat, lon) para o grafo lembrar o mapa. Alguns
   estados muito proximos (Nordeste litoraneo, DF/GO, RJ/ES) foram afastados
   de proposito para os rotulos nao se sobreporem. */
const COORD = {
  AC: [-9.0, -70.5], AM: [-4.0, -64.5], RR: [2.0, -61.0], RO: [-11.0, -63.0],
  PA: [-4.5, -52.5], AP: [1.6, -51.5], TO: [-10.5, -48.0], MA: [-5.0, -45.0],
  PI: [-7.8, -42.3], CE: [-4.3, -39.0], RN: [-4.8, -35.0], PB: [-7.0, -34.6],
  PE: [-8.6, -37.6], AL: [-9.9, -35.2], SE: [-11.2, -37.0], BA: [-12.8, -41.5],
  MT: [-13.0, -56.0], MS: [-20.8, -55.0], GO: [-16.6, -50.2], DF: [-14.6, -46.6],
  MG: [-18.8, -44.0], ES: [-19.9, -39.8], RJ: [-22.9, -43.0], SP: [-22.6, -48.8],
  PR: [-25.2, -51.8], SC: [-27.6, -50.3], RS: [-30.4, -53.5]
};

const D = window.DADOS;

/* faixa de precos para colorir os nos (verde barato -> vermelho caro) */
const precos = D.estados.map(e => e.preco);
const PMIN = Math.min(...precos);
const PMAX = Math.max(...precos);

function corPreco(preco) {
  const t = PMAX > PMIN ? (preco - PMIN) / (PMAX - PMIN) : 0;
  // gradiente verde -> amarelo -> vermelho
  const r = t < 0.5 ? Math.round(46 + (232 - 46) * (t / 0.5))
                    : 232;
  const g = t < 0.5 ? Math.round(204 - (204 - 216) * (t / 0.5))
                    : Math.round(216 - (216 - 59) * ((t - 0.5) / 0.5));
  const b = t < 0.5 ? Math.round(113 - (113 - 74) * (t / 0.5))
                    : Math.round(74 - (74 - 59) * ((t - 0.5) / 0.5));
  return `rgb(${r},${g},${b})`;
}

/* --- monta nos e arestas para o vis-network --- */
const nodes = new vis.DataSet(D.estados.map(e => {
  const [lat, lon] = COORD[e.sigla];
  return {
    id: e.sigla,
    label: e.sigla,
    x: (lon + 75) * 30,
    y: (-lat) * 30,
    fixed: true,
    color: {
      background: corPreco(e.preco),
      border: "#0b101b",
      highlight: { background: corPreco(e.preco), border: "#fff" }
    },
    font: { color: "#ffffff", size: 16, face: "monospace",
            strokeWidth: 4, strokeColor: "#05080f", vadjust: 2 },
    borderWidth: 2,
    title: `${NOMES[e.sigla]} — R$ ${e.preco.toFixed(2)}` +
           (e.amostras ? ` (${e.amostras} postos)` : " (média nacional)")
  };
}));

const BORDA_PADRAO = { color: "#36436a", width: 1, dashes: false };
const edgeId = (a, b) => [a, b].sort().join("|");

const edges = new vis.DataSet(D.arestas.map(([a, b]) => ({
  id: edgeId(a, b),
  from: a,
  to: b,
  color: { color: BORDA_PADRAO.color },
  width: BORDA_PADRAO.width,
  smooth: false
})));

const container = document.getElementById("grafo");
const network = new vis.Network(container, { nodes, edges }, {
  physics: false,
  interaction: { hover: true, dragNodes: false, zoomView: true },
  nodes: { shape: "dot", size: 20 },
  edges: { selectionWidth: 0 }
});

/* --- popula os selects --- */
const selOrigem = document.getElementById("origem");
const selDestino = document.getElementById("destino");
const ordenados = [...D.estados].sort((a, b) =>
  NOMES[a.sigla].localeCompare(NOMES[b.sigla]));

for (const e of ordenados) {
  const txt = `${NOMES[e.sigla]} (${e.sigla})`;
  selOrigem.add(new Option(txt, e.sigla));
  selDestino.add(new Option(txt, e.sigla));
}
selOrigem.value = "DF";
selDestino.value = "AM";

/* --- realce das rotas --- */
function limparRealce() {
  nodes.forEach(n => nodes.update({ id: n.id, borderWidth: 2,
    color: { ...n.color, border: "#0b101b" } }));
  edges.forEach(e => edges.update({ id: e.id,
    color: { color: BORDA_PADRAO.color }, width: BORDA_PADRAO.width,
    dashes: false }));
}

function realcarAresta(a, b, cor, largura, dashes) {
  const id = edgeId(a, b);
  if (edges.get(id)) {
    edges.update({ id, color: { color: cor }, width: largura, dashes });
  }
}

function realcarNo(sigla, cor) {
  const n = nodes.get(sigla);
  nodes.update({ id: sigla, borderWidth: 4,
    color: { ...n.color, border: cor } });
}

/* anima a rota acendendo estado por estado */
async function animarRota(caminho, cor, largura, dashes) {
  for (let i = 0; i < caminho.length; i++) {
    realcarNo(caminho[i], cor);
    if (i > 0) realcarAresta(caminho[i - 1], caminho[i], cor, largura, dashes);
    await new Promise(r => setTimeout(r, 260));
  }
}

/* --- monta o painel lateral --- */
function siglasComPreco(caminho) {
  return caminho.map(s => {
    const e = D.estados.find(x => x.sigla === s);
    return `<span class="uf">${s}<small>R$ ${e.preco.toFixed(2)}</small></span>`;
  }).join('<span class="seta">›</span>');
}

function blocoRota(titulo, classe, rota) {
  return `
    <div class="bloco">
      <h3><span class="tag ${classe}">${titulo}</span></h3>
      <div class="rota-caminho">${siglasComPreco(rota.caminho)}</div>
      <div class="metricas">
        Estados no caminho: <b>${rota.caminho.length}</b> (${rota.saltos} fronteiras)<br>
        Custo total de gasolina: <b>R$ ${rota.custo.toFixed(2)}</b><br>
        Nós/caminhos explorados: <b>${rota.exploracoes}</b>
      </div>
    </div>`;
}

function atualizarPainel(o, d, bfs, barato) {
  const painel = document.getElementById("painel");
  const mesmoCaminho =
    JSON.stringify(bfs.caminho) === JSON.stringify(barato.caminho);

  let html = `<h2 style="margin:0 0 12px;font-size:16px">
      ${NOMES[o]} → ${NOMES[d]}</h2>`;

  html += blocoRota("MAIS BARATA · DFS", "barato", barato);
  html += blocoRota("MENOS ESTADOS · BFS", "curta", bfs);

  if (mesmoCaminho) {
    html += `<div class="iguais">Neste par a rota mais curta
       <b>já é</b> a mais barata.</div>`;
  } else {
    const economia = bfs.custo - barato.custo;
    html += `<div class="economia">Desviar pela rota mais barata economiza
       <b>R$ ${economia.toFixed(2)}</b> em relação à rota mais curta
       (mesmo passando por ${barato.caminho.length - bfs.caminho.length >= 0
         ? (barato.caminho.length - bfs.caminho.length) + " estado(s) a mais"
         : "menos estados"}).</div>`;
  }
  painel.innerHTML = html;
}

/* --- acao do botao --- */
const botao = document.getElementById("tracar");

async function tracar() {
  const o = selOrigem.value;
  const d = selDestino.value;
  if (o === d) {
    document.getElementById("painel").innerHTML =
      `<p class="vazio">Escolha estados diferentes para origem e destino.</p>`;
    return;
  }

  const par = D.rotas[`${o}|${d}`];
  if (!par) return;

  botao.disabled = true;
  limparRealce();

  // primeiro a rota mais curta (azul tracejado), depois a mais barata (verde)
  await animarRota(par.bfs.caminho, "#4aa3ff", 4, true);
  await new Promise(r => setTimeout(r, 200));
  await animarRota(par.barato.caminho, "#2ecc71", 6, false);

  atualizarPainel(o, d, par.bfs, par.barato);
  botao.disabled = false;
}

botao.addEventListener("click", tracar);
network.once("afterDrawing", () => network.fit({ animation: false }));
