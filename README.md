# Monitoramento-de-temperatura-e-umidade-via-WiFi-

<p align="center">
<img width="1376" height="768" alt="Gemini_Generated_Image_qz3ne7qz3ne7qz3n" src="https://github.com/user-attachments/assets/0b606e7a-a659-406b-8f84-ef7fb30f1cc0" />

</p>
<h1 align="center"> Entrega - 04</h1>

##

<p align="center">
  <a href="#requisito">Requisitos </a> 
  |
  <a href="#objetivo">Objetivo da Sprint </a>
  |
  <a href="#desenvolvimento">Desenvolvimento </a>
  |
  <a href="#cronograma">Cronograma </a>
  |
  <a href="#conclusao">Conclusão </a>
  
</p>

##

**Início: 20/11  - Fim: 12/12**

**Status da Sprint: Concluído ✅**
<br>
<br>
<br>

<span id="requisito">
  
 ## 📜Requisitos

Desenvolver um sistema utilizando os microcontroladores ESP32-C3 e ESP32-S3 para monitorar temperatura e umidade via Wi-Fi. Exibir os dados em tempo real e o histórico em uma tela touchscreen, desenvolvida no SquareLine Studio, com configuração de limites de temperatura e acionamento de alertas visuais e sonoros.

<span id="objetivo">
  
## 📌Objetivo da Sprint
O objetivo desta fase é realizar a validação da solução.<br>

<span id="desenvolvimento">
  
## 📚Desenvolvimento


# Introdução <br>
>Atualmente, o monitoramento de temperatura e umidade é cada vez mais importante em ambientes residenciais, comerciais e industriais, contribuindo para a conservação de equipamentos, produtos e para a segurança dos ambientes. Nesse contexto, a utilização de sistemas eletrônicos conectados à internet permite acompanhar as condições ambientais de maneira prática e em tempo real.<br>
>Este projeto tem como objetivo desenvolver um sistema de monitoramento de temperatura e umidade utilizando dois microcontroladores ESP32, sendo um ESP32-C3 responsável pela coleta dos dados por meio de um sensor DHT22 e um ESP32-S3 com tela touchscreen de 7 polegadas, responsável pela visualização e interação com o usuário.<br>
>O sistema permite acompanhar os valores de temperatura e umidade em tempo real, consultar históricos de medições por meio de gráficos e configurar parâmetros de funcionamento, incluindo limite máximo de temperatura e modos de acionamento dos alertas sonoros e visuais. A interface gráfica foi desenvolvida utilizando o SquareLine Studio, proporcionando uma navegação intuitiva e facilitando o acompanhamento das informações.<br>
>A proposta busca integrar eletrônica, programação, comunicação Wi-Fi e desenvolvimento de interfaces gráficas em uma solução tecnológica de monitoramento.<br>

---

# Processo <br>
>Serão descritos abaixo os principais componentes e recursos utilizados para a realização do projeto:<br>

**• ESP32-C3** <br>
>É um microcontrolador utilizado para realizar a leitura dos dados ambientais por meio do sensor DHT22. Também é responsável pelo processamento das informações e pelo envio dos valores de temperatura e umidade através de uma conexão Wi-Fi, disponibilizando os dados para o ESP32-S3.<br>

**• ESP32-S3 Touch LCD 7** <br>
>É o microcontrolador responsável pelo gerenciamento da interface gráfica e pela interação com o usuário. Possui uma tela touchscreen de 7 polegadas, permitindo visualizar os dados recebidos, consultar gráficos históricos e acessar as configurações do sistema.<br>

**• Sensor DHT22** <br>
>É um sensor digital utilizado para medir a temperatura e a umidade relativa do ar. Ele realiza as medições do ambiente e envia os dados ao ESP32-C3, que processa e disponibiliza essas informações para a interface de monitoramento.<br>

**• Display Touchscreen de 7 polegadas** <br>
>É o dispositivo responsável pela apresentação visual das informações do sistema. Por meio da tela sensível ao toque, o usuário consegue navegar entre as páginas, acompanhar as medições e realizar ajustes nas configurações.<br>

**• LED indicador** <br>
>É um componente utilizado para fornecer sinalização visual do funcionamento do sistema. Seu acionamento está relacionado às condições de monitoramento e aos alertas configurados.<br>

**• Buzzer** <br>
>É um dispositivo eletrônico responsável pela emissão de sinais sonoros quando ocorre uma condição de alerta. Seu funcionamento pode ser configurado nos modos automático ou manual, conforme as opções disponíveis na interface.<br>

**• Protoboard e componentes de conexão** <br>
>São utilizados para realizar as ligações elétricas entre os componentes do sistema, facilitando a montagem, os testes e possíveis alterações no circuito.<br>

**• Comunicação Wi-Fi** <br>
>É o recurso responsável pela comunicação sem fio entre os dois microcontroladores. Permite que os dados coletados pelo ESP32-C3 sejam enviados ao ESP32-S3 para apresentação na tela touchscreen.<br>

**• SquareLine Studio e LVGL** <br>
>São ferramentas utilizadas no desenvolvimento da interface gráfica. O SquareLine Studio permite criar visualmente as telas, botões, indicadores e gráficos, enquanto a biblioteca LVGL possibilita implementar a interface no ESP32-S3 e controlar seus elementos.<br>

---

# Funcionamento Geral

**• Coleta de dados:** <br>
>O sensor DHT22 realiza continuamente a leitura da temperatura e da umidade relativa do ambiente.

**• Processamento:** <br>
>O ESP32-C3 recebe os valores fornecidos pelo sensor, processa as informações e prepara os dados para transmissão.

**• Comunicação Wi-Fi:** <br>
>O ESP32-C3 disponibiliza os dados por meio de uma conexão Wi-Fi, permitindo que o ESP32-S3 acesse as informações para atualizar a interface gráfica.<br>

**• Recepção e visualização:** <br>
>O ESP32-S3 recebe os valores de temperatura e umidade e atualiza as informações apresentadas na tela touchscreen de 7 polegadas, permitindo o acompanhamento em tempo real.<br>

**• Histórico de medições:** <br>
>O sistema permite apresentar gráficos com o histórico das medições, facilitando a observação das variações de temperatura ao longo do tempo.<br>

**• Configuração de temperatura máxima:** <br>
>O usuário pode definir um limite máximo de temperatura por meio da interface. Quando a temperatura atinge ou ultrapassa o valor configurado, o sistema identifica a condição de alerta.<br>

**• Acionamento dos alertas:** <br>
>Quando o limite de temperatura é atingido, o sistema pode acionar o LED e o buzzer no modo automático. Também existe a possibilidade de controlar os alertas manualmente pela interface.<br>

---

# Esquema de Ligação <br>
>O projeto é dividido em duas partes principais: o módulo de aquisição de dados e o módulo de visualização.<br>
>O ESP32-C3 está conectado ao sensor DHT22, responsável pela medição da temperatura e umidade do ambiente. Também estão conectados ao microcontrolador um LED indicador e um buzzer, utilizados para os alertas visuais e sonoros.<br>
>O ESP32-S3 está integrado à tela touchscreen de 7 polegadas, responsável pela apresentação dos dados e pelas configurações do sistema.<br>
>A comunicação entre os dois módulos ocorre por meio de Wi-Fi, permitindo transmitir os valores coletados pelo sensor até a interface gráfica.<br>

---

# Funcionamento por Etapas

**• Sensor DHT22:** <br>
>O sensor DHT22 é responsável por realizar as medições ambientais.<br>

- Mede a temperatura do ambiente em graus Celsius (°C);<br>
- Mede a umidade relativa do ar em porcentagem (%);<br>
- Envia os valores digitais ao ESP32-C3 por meio do pino de dados;<br>
- As medições são utilizadas para atualizar as informações do sistema.<br>

**• ESP32-C3 – Aquisição e transmissão dos dados:** <br>
>O ESP32-C3 recebe as informações do sensor DHT22 e realiza o processamento dos valores.

- Inicializa o sensor e a comunicação Wi-Fi;<br>
- Realiza as leituras de temperatura e umidade;<br>
- Disponibiliza os dados por meio de um servidor web local;<br>
- Permite que o ESP32-S3 consulte as informações para atualização da interface.<br>

**• ESP32-S3 – Interface de monitoramento:** <br>
>O ESP32-S3 recebe os dados enviados pelo ESP32-C3 e gerencia a apresentação das informações na tela.<br>
- Atualiza os valores de temperatura e umidade em tempo real;<br>
- Apresenta os dados de forma organizada na tela touchscreen;<br>
- Permite navegar entre as páginas do sistema;<br>
- Disponibiliza gráficos para consulta do histórico de medições;<br>
- Permite acessar as configurações dos alertas.<br>

**• Interface gráfica – SquareLine Studio e LVGL:** <br>
>A interface foi desenvolvida visualmente no SquareLine Studio e integrada ao projeto por meio da biblioteca LVGL.

- Página inicial: apresenta os valores atuais de temperatura e umidade;<br>
- Página de histórico: apresenta gráficos com as variações das medições;<br>
- Página de configurações: permite ajustar o limite máximo de temperatura e selecionar o modo de funcionamento dos alertas;<br>
- Controles touchscreen: permitem interagir com os botões e demais elementos da interface.<br>

**• Controle automático e manual dos alertas:** <br>
O sistema possui duas formas de controle:<br>
- **Modo automático:** quando a temperatura atinge ou ultrapassa o limite configurado, o sistema aciona os indicadores visual e sonoro.<br>
- **Modo manual:** permite ao usuário ligar ou desligar os alertas diretamente pela interface touchscreen, utilizando o botão de controle.<br>

**• Acionamento do LED e buzzer:** <br>
>O LED e o buzzer são utilizados como indicadores das condições de alerta do sistema.<br>
- O LED fornece uma indicação visual;<br>
- O buzzer emite um sinal sonoro;<br>
- No modo automático, os componentes são acionados conforme o limite de temperatura configurado;<br>
- No modo manual, o usuário controla o acionamento pela tela.<br>

**• Resumo do Fluxo:**
*Sensor DHT22 realiza a leitura da temperatura e umidade → ESP32-C3 recebe e processa os dados → informações são disponibilizadas por Wi-Fi → ESP32-S3 recebe os valores → interface touchscreen atualiza as informações → usuário acompanha as medições e consulta os gráficos → caso a temperatura atinja o limite configurado, os alertas são acionados no modo automático ou podem ser controlados manualmente pela interface.* <br>

### Roteiro de Montagem
>A montagem do sistema foi realizada por meio da conexão dos componentes eletrônicos aos microcontroladores ESP32-C3 e ESP32-S3. Foram utilizados fios jumpers e protoboard para as conexões, além de ferramentas como ferro de solda, fio de estanho, chave de fenda e pinça, quando necessário.<br>


### Montagem Final

**• Montagem do Circuito** <br>
<img width="479" height="481" alt="circuito" src="https://github.com/user-attachments/assets/043e40c2-3336-44d8-a789-d76e84c25f6c" />
<br>
<img width="900" height="1600" alt="WhatsApp Image 2026-10-02 at 16 06 50" src="https://github.com/user-attachments/assets/688e5440-226f-43f4-9e37-e4bfe97036da" />


### Design Final
https://github.com/user-attachments/assets/e261ff6e-b604-4f39-8951-d5cd40569fe1




## 📅Cronograma

![Sprint4]


<span id="conclusao">
## ✅Conclusão

*Com a finalização deste projeto, foi possível comprovar o funcionamento eficaz do sensor óptico de barreira, atendendo plenamente aos objetivos inicialmente propostos. O sistema demonstrou ser funcional, de fácil implementação e capaz de detectar de forma precisa a interrupção de objetos na barreira de luz, ativando os sinais sonoro e luminoso conforme planejado. Durante o desenvolvimento, a colaboração entre os integrantes da equipe foi essencial, promovendo um ambiente de trabalho cooperativo e produtivo. O comprometimento coletivo foi determinante para a superação dos desafios encontrados ao longo do processo, especialmente nas etapas de montagem e soldagem dos componentes na placa de circuito impresso, que exigiram atenção, paciência e precisão.*

> 

#### 📝Documentação:  [Clique Aqui](https://github.com/MavPro-tech/Monitoramento-via-WiFi/tree/main/Sprints/Sprint_04)

