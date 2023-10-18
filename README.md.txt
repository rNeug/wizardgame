Renato Nagassaki de Melo - 11202131708

# Wizards Game

**Wizards Game** é um jogo simples desenvolvido em C++ com o framework [ABCG](https://github.com/abcg-dev/abcg) que envolve magos, feitiços e estrelas.

## Sobre o Jogo

Neste jogo, você controla um mago e precisa evitar as estrelas enquanto lança feitiços para eliminar outros magos que aparecem na tela. Você ganha o jogo ao eliminar todos os magos, e perde se colidir com alguma estrela ou mago inimigo.

## Funcionalidades

- Movimentação do mago com as teclas WASD ou as setas do teclado.
- Lançamento de feitiços com o botão esquerdo do mouse.
- Troca de cor do mago com as teclas numéricas 1, 2, 3 e 4.
- Efeitos visuais como rastros de movimento e explosões de feitiços.

## Controles

- Setas direcionais ou teclas WASD: Movimentar o mago.
- Botão esquerdo do mouse: Lançar feitiços.
- Teclas numéricas 1, 2, 3 e 4: Trocar a cor do mago.

## Requisitos de Compilação

- [ABCG Framework](https://github.com/abcg-dev/abcg)
- [OpenGL](https://www.opengl.org/)

Wizards (Wizards.hpp e Wizards.cpp):

Responsável por representar os inimigos no jogo (chamados "Wizards").
Utiliza OpenGL para renderizar os inimigos e gerencia suas posições, velocidades, e outras propriedades.
Gera os atributos de geometria e as VAOs (Vertex Array Objects) associadas aos inimigos.
Window (Window.hpp e Window.cpp):

Representa a janela da aplicação e gerencia o ciclo de vida do jogo.
Manipula os eventos do SDL (teclado, mouse) e atualiza o estado do jogo conforme as interações do jogador.
Renderiza os elementos do jogo, como estrelas, feitiços, inimigos, e a entidade controlada pelo jogador.
Spells (Spells.hpp e Spells.cpp):

Representa os feitiços que o jogador pode lançar para atacar os inimigos.
Utiliza OpenGL para renderizar os feitiços e gerencia suas posições e velocidades.
Gera os atributos de geometria e as VAOs associadas aos feitiços.
HP (HP.hpp e HP.cpp):

Representa a entidade controlada pelo jogador.
Utiliza OpenGL para renderizar a entidade e gerencia suas posições, velocidades, cores, etc.
Gera os atributos de geometria e as VAOs associadas à entidade.

## Compilando e Executando

1. Certifique-se de que o ABCG Framework e o OpenGL estão instalados.
2. Clone o repositório do jogo.
3. Abra o projeto no seu ambiente de desenvolvimento C++.
4. Compile e execute o jogo.

## Contribuição

Sinta-se à vontade para abrir problemas ou enviar solicitações de recebimento se desejar contribuir para este projeto. Agradecemos a sua colaboração!

## Autor

Este jogo foi criado por [Seu Nome](https://github.com/seu-usuario).

## Licença

Este projeto é licenciado sob a Licença MIT - consulte o arquivo [LICENSE](LICENSE) para obter detalhes.

---

Divirta-se jogando **Wizards Game**!
