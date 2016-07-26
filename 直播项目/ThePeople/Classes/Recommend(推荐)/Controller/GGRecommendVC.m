//
//  GGRecommendVC.m
//  ThePeople
//
//  Created by aoyolo on 16/4/12.
//  Copyright © 2016年 Ms.Gao. All rights reserved.
//

#import "GGRecommendVC.h"
#import "GGPlayViewCtl.h"

#import "GGBaseCell.h"
#import "GGMovieCell.h"
#import "GGRecomScrollCell.h"
#import "GGRecomHeaderView.h"
#import "GGListClassCell.h"


#import "GGMovieModel.h"
#import "GGRecomHeaderModel.h"
#import "GGRecomScrModel.h"
#import "GGListClassModel.h"
#import "MJRefresh.h"

@interface GGRecommendVC ()<UICollectionViewDataSource,UICollectionViewDelegate,UICollectionViewDelegateFlowLayout>
@property(nonatomic , strong) UICollectionView *collectionView;
@property(nonatomic , strong) UICollectionViewFlowLayout *flowLayout;
@property(nonatomic , strong) NSMutableArray  *tempArrA;
@property(nonatomic , strong) NSMutableArray  *tempArrB;
@property(nonatomic , strong) NSMutableArray  *tempArrC;
@property(nonatomic , strong) NSMutableArray  *tempArrD;
@property(nonatomic , strong) NSMutableDictionary *dataDic;
@end

@implementation GGRecommendVC

#pragma mark 懒加载存放数据的数组
-(NSMutableArray *)tempArrA{
    if (_tempArrA == nil) {
        _tempArrA = [[NSMutableArray alloc]init];
    }
    return _tempArrA;
}
-(NSMutableArray *)tempArrB{
    if (_tempArrB == nil) {
        _tempArrB = [[NSMutableArray alloc]init];
    }
    return _tempArrB;
}
-(NSMutableArray *)tempArrC{
    if (_tempArrC == nil) {
        _tempArrC = [[NSMutableArray alloc]init];
    }
    return _tempArrC;
}
-(NSMutableArray *)tempArrD{
    if (_tempArrD == nil) {
        _tempArrD = [[NSMutableArray alloc]init];
    }
    return _tempArrD;
}
-(NSMutableDictionary *)dataDic{
    if (_dataDic == nil) {
        _dataDic = [NSMutableDictionary dictionary];
    }
    return _dataDic;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    [self reloadCollectionView];
    [self registerCell];
    [self httpRequest];
    
    [self.collectionView addHeaderWithTarget:self action:@selector(httpRequest)];
    [self.collectionView headerBeginRefreshing];
}

#pragma mark -添加collectionView
-(void)reloadCollectionView{
    UICollectionViewFlowLayout *flowLayout = [[UICollectionViewFlowLayout alloc] init];
    UICollectionView *collectionView = [[UICollectionView alloc] initWithFrame:self.view.bounds collectionViewLayout:flowLayout];
    self.flowLayout = flowLayout;
    self.collectionView = collectionView;
    self.collectionView.delegate = self;
    self.collectionView.dataSource = self;
    [self.view addSubview:self.collectionView];
    self.collectionView.backgroundColor = [UIColor whiteColor];
    
}
#pragma mark -注册cell
- (void)registerCell{
    [self.collectionView registerClass:[GGRecomScrollCell class] forCellWithReuseIdentifier:@"GGRecomScrollCell"];
    [self.collectionView registerClass:[GGListClassCell class] forCellWithReuseIdentifier:@"GGListClassCell"];
    [self.collectionView registerNib:[UINib nibWithNibName:@"GGMovieCell" bundle:nil] forCellWithReuseIdentifier:@"GGMovieCell"];
    [self.collectionView registerNib:[UINib nibWithNibName:@"GGRecomHeaderView" bundle:nil] forSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@"GGRecomHeaderView"];
}

#pragma mark -解析网络数据
- (void)httpRequest{
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
    [manager GET:RecommendUrl parameters:nil success:^(AFHTTPRequestOperation * _Nonnull operation, id  _Nonnull responseObject) {
        if (self.dataDic) {
            [self.dataDic removeAllObjects];
            [self.tempArrA removeAllObjects];
            [self.tempArrB removeAllObjects];
            [self.tempArrC removeAllObjects];
            [self.tempArrD removeAllObjects];
            
        }
        self.tempArrA = [NSMutableArray array];
        for (NSDictionary *dic in responseObject[@"app-index"]) {
            GGRecomScrModel *scrM = [[GGRecomScrModel alloc]init];
            scrM.title = dic[@"title"];
            scrM.thumb = dic[@"thumb"];
            [self.tempArrA addObject:scrM];
        }
        
        self.tempArrB = [NSMutableArray array];
        for (NSDictionary *dic in responseObject[@"app-classification"]) {
            GGListClassModel *classM = [[GGListClassModel alloc]init];
            classM.title = dic[@"title"];
            classM.thnumb = dic[@"thumb"];
            [self.tempArrB addObject:classM];
        }
        
        self.tempArrC = [NSMutableArray array];
        for (NSDictionary *dic in responseObject[@"list"]) {
            GGRecomHeaderModel *headerModel = [[GGRecomHeaderModel alloc]init];
            headerModel.name = dic[@"name"];
            headerModel.slug = dic[@"slug"];
            [self.tempArrC addObject:headerModel];
            
        }
        
        NSArray *classArr = @[@"app-recommendation",@"app-lol",@"app-beauty",@"app-webgame",@"app-dota2",@"app-heartstone",@"app-tvgame",@"app-blizzard",@"app-sport",@"app-dnf",@"app-minecraft"];
        self.tempArrD = [NSMutableArray array];
        for (int i = 0; i<classArr.count; i++) {
            NSMutableArray *tempArr = [NSMutableArray array];
            for (NSDictionary *Dic in responseObject[classArr[i]])
            {
                GGMovieModel *GameM = [[GGMovieModel alloc]init];
                GameM.avatar = Dic[@"link_object"][@"avatar"];
                GameM.thumb  = Dic[@"link_object"][@"thumb"];
                GameM.title  = Dic[@"link_object"][@"title"];
                GameM.nick   = Dic[@"link_object"][@"nick"];
                GameM.view   = Dic[@"link_object"][@"view"];
                GameM.uid    = Dic[@"link_object"][@"uid"];
                [tempArr addObject:GameM];
            }
            if (tempArr) {
                [self.tempArrD addObject:tempArr];
            }
        }
        [self.dataDic setObject:self.tempArrA forKey:@"A"];
        [self.dataDic setObject:self.tempArrB forKey:@"B"];
        [self.dataDic setObject:self.tempArrC forKey:@"C"];
        [self.dataDic setObject:self.tempArrD forKey:@"D"];
        [self.collectionView reloadData];
        [self.collectionView headerEndRefreshing];
        
    } failure:^(AFHTTPRequestOperation * _Nonnull operation, NSError * _Nonnull error) {
        NSLog(@"%@",error);
    }];
}
#pragma mark <UICollectionViewDataSource>
- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return self.tempArrD.count+2;
}


- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    if (section == 2) {
        return 2;
    }else if (section > 2) {
        NSArray *arr = self.tempArrD[section-2];
        if (arr.count>=2&&arr.count < 4) {
            return 2;
        }else if (arr.count>=4){
            return 4;
        }
        
    }
    return 1;
}

-(CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.section == 0) {
        return CGSizeMake(ScreenWidth, ScreenWidth/7*3);
    }else if(indexPath.section == 1){
        return CGSizeMake(ScreenWidth, ScreenWidth/5+20);
    }
    return CGSizeMake((ScreenWidth-20)/2, (ScreenWidth-20)/2-10);
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *cellID = nil;
    if (indexPath.section == 0) {
        cellID = @"GGRecomScrollCell";
    }else if (indexPath.section == 1){
        cellID = @"GGListClassCell";
    }else{
        cellID = @"GGMovieCell";
    }
    GGBaseCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:cellID forIndexPath:indexPath];
    if (indexPath.section == 0) {
        [cell setCellModel:self.tempArrA];
    }else if (indexPath.section == 1){
        [cell setCellModel:self.tempArrB];
    }else if (indexPath.section >1 ) {
        NSArray *arr = self.tempArrD[indexPath.section-2];
        GGMovieModel *model = arr[indexPath.row];
        [cell setCellModel:model];
    }
    return cell;
}

- (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section{
    if (section == 0||section == 1) {
        return UIEdgeInsetsMake(0, 0, 0, 0);
    }
    return UIEdgeInsetsMake(5, 5, 5, 5);
}

- (UICollectionReusableView *)collectionView:(UICollectionView *)collectionView viewForSupplementaryElementOfKind:(NSString *)kind atIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.section > 1) {
        if ([kind isEqualToString:UICollectionElementKindSectionHeader]) {
            GGRecomHeaderView *reusableView = [collectionView dequeueReusableSupplementaryViewOfKind:UICollectionElementKindSectionHeader withReuseIdentifier:@"GGRecomHeaderView" forIndexPath:indexPath];
            if (indexPath.section == 2) {
                [reusableView.goUp setTitle:@"换一换" forState:UIControlStateNormal];
            }else{
                [reusableView.goUp setTitle:@"进去看看" forState:UIControlStateNormal];
            }
            reusableView.recomHeaderModel = self.tempArrC[indexPath.section];
            return reusableView;
        }
    }
    return nil;
}
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout referenceSizeForHeaderInSection:(NSInteger)section{
    if (section >1) {
        return CGSizeMake(ScreenWidth, 40);
    }
    return CGSizeMake(0, 0);
}

#pragma mark <UICollectionViewDelegate>
- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.section >1 ) {
        NSArray *arr = self.tempArrD[indexPath.section-2];
        GGMovieModel *model = arr[indexPath.row];
        GGPlayViewCtl *playCtl= [[GGPlayViewCtl alloc]initWithVideoId:model.uid];
        playCtl.hidesBottomBarWhenPushed = YES;
        [self.navigationController pushViewController:playCtl animated:NO];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

@end
