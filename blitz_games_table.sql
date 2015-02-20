use chess

drop table if exists blitz_games;

create table blitz_games (
  id mediumint auto_increment primary key,
  blitz_game_date date not null,
  time_control varchar(10) not null,
  opponent_name varchar(25) not null,
  color varchar(1) not null,
  num_moves smallint not null,
  result varchar(1) not null,
  result_decimal decimal(6,1) null,
  result_detail varchar(25) not null,
  opponent_elo_before int null,
  opponent_elo_after int not null,
  opponent_elo_delta smallint null,
  my_elo_before int null,
  my_elo_after int not null,
  my_elo_delta smallint null,
  first_move varchar(10) null,
  first_two_moves varchar(25) null,
  first_three_moves varchar(35) null,
  first_four_moves varchar(45) null,
  opening varchar(10) null
);

quit
